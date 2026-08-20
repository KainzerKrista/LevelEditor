#include "Renderer.h"

#include <d3dcompiler.h>
#include<cstddef>
#include<filesystem>
#include <iostream>
#include<iterator>

namespace 
{ 
	// match semantics in HLSL
	struct Vertex
	{
		// x, y, z
		float position[3];
		
		// red, green blue, alpha
		float color[4];
	};

	// Test Triangle
	constexpr Vertex TriangleVertices[] =
	{
		{
			{0.0f, 0.5f, 0.0f},
			{1.0f, 0.0f, 0.0f, 1.0f}
		},

		{
			{-0.5f, -0.5f, 0.0f},
			{0.0f, 1.0f, 0.0f, 1.0f}
		},

		{
			{0.5f, -0.5f, 0.0f},
			{0.0f, 0.0f, 1.0f, 1.0f}
		}

	};

	std::filesystem::path GetExecutableDirectory()
	{
		wchar_t path[MAX_PATH]{};

		DWORD length = GetModuleFileNameW(nullptr, path, MAX_PATH);
		if (length == 0)
		{
			return {};
		}

		return std::filesystem::path(path).parent_path();
	}

	// Shader Compiler helper
	bool CompileShader(const std::filesystem::path& shaderPath, const char* entryPoint, const char* target, Microsoft::WRL::ComPtr<ID3DBlob>& shaderBlob)
	{
		UINT compilerFlags = 0;

#ifdef _DEBUG
		compilerFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob; 
		HRESULT result = D3DCompileFromFile(shaderPath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint, target, compilerFlags, 0, &shaderBlob, &errorBlob);

		if (FAILED(result))
		{
			std::cerr
				<< "ERROR: Failed to compile shader: "
				<< shaderPath.string()
				<< '\n';
			
			if (errorBlob)
			{
				std::cerr
					<< static_cast<const char*>(errorBlob->GetBufferPointer())
					<< '\n';
			}

			return false;
		}

		return true;
	}
}

bool Renderer::Initialize(HWND windowHandle, int width, int height)
{
	m_width = width;
	m_height = height;

	// Swap Chain Description
	DXGI_SWAP_CHAIN_DESC swapChainDescription{};

	// Sets RGBA to 8 bits
	swapChainDescription.BufferDesc.Width = width;
	swapChainDescription.BufferDesc.Height = height;
	swapChainDescription.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	swapChainDescription.SampleDesc.Count = 1;
	swapChainDescription.SampleDesc.Quality = 0;

	// Set Target location for buffers
	swapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDescription.BufferCount = 2;

	// Handles Window sizing
	swapChainDescription.OutputWindow = windowHandle;
	swapChainDescription.Windowed = TRUE;
	swapChainDescription.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Sets Direct3D compatibilities
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};

	D3D_FEATURE_LEVEL selectedFeatureLevel{};

	// Initialise Direct3D 11 for GPU device, GPU command context, and display swap chains
	HRESULT result = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		featureLevels,
		static_cast<UINT>(std::size(featureLevels)),
		D3D11_SDK_VERSION,
		&swapChainDescription,
		&m_swapChain,
		&m_device,
		&selectedFeatureLevel,
		&m_deviceContext
	);

	if (FAILED(result))
	{
		std::cerr << "ERROR: Failed to create D3D11 device and swap chain.\n";
		return false;
	}

	// Creates a view into The Back Buffer
	if (!CreateRenderTarget())
	{
		return false;
	}

	UpdateViewport(width, height);

	if (!CreateTrianglePipeline())
	{
		return false;
	}

	return true;
}

bool Renderer::CreateRenderTarget()
{
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;

	HRESULT result = m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
	
	if (FAILED(result))
	{
		std::cerr << "ERROR: Failed to get swap chain back buffer\n";
		return false;
	}

	// Informs D3D11 how to use the created texture.
	result = m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, &m_renderTargetView);

	if (FAILED(result))
	{
		std::cerr << "ERROR: Failed to create render target view\n";
		return false;
	}

	return true;
}

void Renderer::DestroyRenderTarget()
{
	if (m_deviceContext)
	{
		// Stops using the current render target
		m_deviceContext->OMSetRenderTargets(0, nullptr, nullptr);
	}

	m_renderTargetView.Reset();
}

// Test Triangle Rendering
bool Renderer::CreateTrianglePipeline()
{
	const std::filesystem::path shaderPath = GetExecutableDirectory()
		/ "assets"
		/ "shaders"
		/ "Triangle.hlsl";

	std::cout
		<< "Loaading shader from: "
		<< shaderPath.string()
		<< '\n';

	// Create and compile Vertex Shader
	Microsoft::WRL::ComPtr<ID3DBlob> vertexShaderBlob;

	if (!CompileShader(shaderPath, "VSMain", "vs_5_0", vertexShaderBlob))
	{
		return false;
	}

	HRESULT result = m_device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, &m_vertexShader);

	if (FAILED(result))
	{
		std::cerr << "ERROR: Failed to create vertex shader.\n";

		return false;
	}

	std::cout << "Vertex shader compiled successfully\n";

	// Create and compile Pixel Shader
	Microsoft::WRL::ComPtr<ID3DBlob> pixelShaderBlob;

	if (!CompileShader(shaderPath, "PSMain", "ps_5_0", pixelShaderBlob))
	{
		return false;
	}

	 result = m_device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, &m_pixelShader);

	if (FAILED(result))
	{
		std::cerr << "ERROR: Failed to create pixel shader.\n";

		return false;
	}

	std::cout << "Pixel Shader compiled successfully\n";

	// Description
	D3D11_INPUT_ELEMENT_DESC inputElements[] =
	{
		// DXGI_FORMAT_R32G32B32_FLOAT and DXGI_FORMAT_R32G32B32A32_FLOAT means using 32-bit floats
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, static_cast<UINT>(offsetof(Vertex, position)), D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, static_cast<UINT>(offsetof(Vertex, color)), D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	// Create and compile input layout
	result = m_device->CreateInputLayout(inputElements, static_cast<UINT>(std::size(inputElements)), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &m_inputLayout);

	if (FAILED(result))
	{
		std::cerr << "ERROR: Failed to create input layout\n";
		return false;
	}

	std::cout << "Input Laayout created successfully\n";

	D3D11_BUFFER_DESC bufferDescription{};

	bufferDescription.ByteWidth = static_cast<UINT>(sizeof(TriangleVertices));
	bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	
	D3D11_SUBRESOURCE_DATA initialData{};

	initialData.pSysMem = TriangleVertices;

	result = m_device->CreateBuffer(&bufferDescription, &initialData, &m_vertexBuffer);

	if (FAILED(result))
	{
		std::cerr << "ERROR: Failed to create vertex buffer\n";
		return false;
	}

	std::cout << "Vertex Buffer created successfully\n";

	// Rasterizer Description
	D3D11_RASTERIZER_DESC rasterizerDescription{};

	rasterizerDescription.FillMode = D3D11_FILL_SOLID;
	rasterizerDescription.CullMode = D3D11_CULL_NONE;
	rasterizerDescription.DepthClipEnable = TRUE;

	result = m_device->CreateRasterizerState(&rasterizerDescription, &m_rasterizerState);

	if (FAILED(result))
	{
		std::cerr << "ERROR: Failed to create rasterizer state\n";

		return false;
	}

	return true;
}

void Renderer::DrawTriangle()
{
	if (!m_vertexBuffer || !m_vertexShader || !m_pixelShader || !m_inputLayout)
	{
		return;
	}

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	ID3D11Buffer* vertexBuffers[] = { m_vertexBuffer.Get() };

	// Input Assembler
	m_deviceContext->IASetVertexBuffers(0, 1, vertexBuffers, &stride, &offset);
	m_deviceContext->IASetInputLayout(m_inputLayout.Get());
	m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Vertex Shader
	m_deviceContext->VSSetShader(m_vertexShader.Get(), nullptr, 0);

	// Pixel Shader
	m_deviceContext->PSSetShader(m_pixelShader.Get(), nullptr, 0);

	m_deviceContext->RSSetState(m_rasterizerState.Get());

	// Draw
	m_deviceContext->Draw(3, 0);
}

void Renderer::Resize(int width, int height)
{
	// Returns if window size is 0x0
	if (width <= 0 || height <= 0)
	{
		return;
	}

	if (!m_swapChain)
	{
		return;
	}

	// Don't rebuild swap chain if window size hasn't be changed
	if (width == m_width && height == m_height)
	{
		return;
	}

	m_width = width;
	m_height = height;

	// Release references to old Back Buffer
	DestroyRenderTarget();

	// Resize the Swap Chain buffers
	HRESULT result = m_swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
	
	if (FAILED(result))
	{
		std::cerr << "ERROR: Failed to resize swap chain\n";
		return;
	}

	// Creates a new Render Target View if Back Buffer has been deleted
	if (!CreateRenderTarget())
	{
		std::cerr << "ERROR: Failed to recreate render target after resize\n";
		return;
	}

	UpdateViewport(width, height);

}	

void Renderer::UpdateViewport(int width, int height)
{
	D3D11_VIEWPORT viewport{};

	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);

	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	m_deviceContext->RSSetViewports(1, &viewport);
}

void Renderer::BeginFrame(float red, float green, float blue, float alpha)
{
	if (!m_renderTargetView)
	{
		return;
	}

	ID3D11RenderTargetView* renderTargets[] = { m_renderTargetView.Get() };
	
	m_deviceContext->OMSetRenderTargets(1, renderTargets, nullptr);
	
	const float clearColor[4] = { red, green, blue, alpha };
	
	m_deviceContext->ClearRenderTargetView(m_renderTargetView.Get(), clearColor);
}

void Renderer::EndFrame()
{
	if (!m_swapChain)
	{
		return;
	}

	m_swapChain->Present(1, 0);
}

void Renderer::Shutdown()
{
	DestroyRenderTarget();

	m_vertexBuffer.Reset();
	m_inputLayout.Reset();

	m_pixelShader.Reset();
	m_vertexShader.Reset();

	m_swapChain.Reset();
	m_deviceContext.Reset();
	m_device.Reset();
}
