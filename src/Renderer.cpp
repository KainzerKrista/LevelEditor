#include "Renderer.h"

#include <d3dcompiler.h>

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#include <cstdint>
#include<cstddef>
#include<filesystem>
#include <iostream>
#include<iterator>

namespace 
{ 


	// GLM
	struct alignas(16) TransformBuffer
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 projection;
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

	if (!CreateDepthBuffer(width, height))
	{
		return false;
	}

	UpdateViewport(width, height);

	if (!CreateMeshPipeline())
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

bool Renderer::CreateDepthBuffer(int width, int height)
{
	D3D11_TEXTURE2D_DESC depthBufferDescription{};
	
	depthBufferDescription.Width = width;
	depthBufferDescription.Height = height;
	depthBufferDescription.MipLevels = 1;
	depthBufferDescription.ArraySize = 1;
	depthBufferDescription.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDescription.SampleDesc.Count = 1;
	depthBufferDescription.SampleDesc.Quality = 0;
	depthBufferDescription.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDescription.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	
	// GPU Texture containing depth values
	HRESULT result = m_device->CreateTexture2D(&depthBufferDescription, nullptr, &m_depthStencilBuffer);

	if (FAILED(result))
	{
		std::cerr << "ERROR: Failed to create depth stencil buffer\n";
		return false;
	}

	// Informs D3D this texture is for depth/stencil usage
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer.Get(), nullptr, &m_depthStencilView);

	if (FAILED(result))
	{
		std::cerr << "ERROR: Failed to create depth stencil view\n";
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

void Renderer::DestroyDepthBuffer()
{
	m_depthStencilView.Reset();
	m_depthStencilBuffer.Reset();
}

// Test Cube Rendering
bool Renderer::CreateMeshPipeline()
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

	std::cout << "Input Layout created successfully\n";

	// Create Constant Buffer
	D3D11_BUFFER_DESC transformBufferDescription{};
	
	transformBufferDescription.ByteWidth = static_cast<UINT>(sizeof(TransformBuffer));
	transformBufferDescription.Usage = D3D11_USAGE_DEFAULT; // These metrix will move constantly
	transformBufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	result = m_device->CreateBuffer(&transformBufferDescription, nullptr, &m_transformBuffer);

	if (FAILED(result))
	{
		std::cerr << "ERROR: Failed to create transform constant buffer\n";
		return false;
	}

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

bool Renderer::CreateMesh(Mesh& mesh, std::span<const Vertex> vertices, std::span<const std::uint16_t> indices)
{
	if (!m_device)
	{
		std::cerr << "ERROR: Mesh cannot be created without renderer initialization\n";
		return false;
	}

	return mesh.Initialize(m_device.Get(), vertices, indices);
}

void Renderer::DrawMesh(const Mesh& mesh, const glm::mat4& model)
{
	if (!mesh.GetVertexBuffer() || !mesh.GetIndexBuffer() || !m_vertexShader || !m_pixelShader || !m_inputLayout || !m_transformBuffer)
	{
		return;
	}
	
	// Camera Postiion, Target, Up
	glm::mat4 view = glm::lookAtLH(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	// Projection Matrix
	const float aspectRatio = static_cast<float>(m_width) / static_cast<float>(m_height);

	glm::mat4 projection = glm::perspectiveLH_ZO(glm::radians(60.0f), aspectRatio, 0.1f, 100.0f); //FOV, Window wxh, near clipping plane, far clipping plane

	// Fill Constant Buffer structure
	TransformBuffer transforms{};

	transforms.model = model;
	transforms.view = view;
	transforms.projection = projection;

	// Upload transform data to GPU
	m_deviceContext->UpdateSubresource(m_transformBuffer.Get(), 0, nullptr, &transforms, 0, 0);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	ID3D11Buffer* vertexBuffers[] = { mesh.GetVertexBuffer()};

	// Input Assembler
	m_deviceContext->IASetVertexBuffers(0, 1, vertexBuffers, &stride, &offset);
	m_deviceContext->IASetIndexBuffer(mesh.GetIndexBuffer(), DXGI_FORMAT_R16_UINT, 0); // Using R16 because std::uint16_t is being used
	m_deviceContext->IASetInputLayout(m_inputLayout.Get());
	m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Vertex Shader
	m_deviceContext->VSSetShader(m_vertexShader.Get(), nullptr, 0);

	// Bind Constant Buffer to Vertext Shader
	ID3D11Buffer* constantBuffers[] = { m_transformBuffer.Get() };
	m_deviceContext->VSSetConstantBuffers(0, 1, constantBuffers); // 0 = register(b0) slot

	// Pixel Shader
	m_deviceContext->PSSetShader(m_pixelShader.Get(), nullptr, 0);

	m_deviceContext->RSSetState(m_rasterizerState.Get());

	// Draw
	m_deviceContext->DrawIndexed(mesh.GetIndexCount(), 0, 0); // number of indicies, start index, base vertex
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
	DestroyDepthBuffer();

	// Resize the Swap Chain buffers
	HRESULT result = m_swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
	
	if (FAILED(result))
	{
		std::cerr << "ERROR: Failed to resize swap chain\n";
		return;
	}

	if (!CreateRenderTarget())
	{
		std::cerr << "ERROR: Failed to recreate render target after resize\n";
		return;
	}

	if (!CreateDepthBuffer(width, height))
	{
		std::cerr << "ERROR: Failed to recreate depth buffer after resize\n";
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
	if (!m_renderTargetView || !m_depthStencilView)
	{
		return;
	}

	ID3D11RenderTargetView* renderTargets[] = { m_renderTargetView.Get() };
	
	m_deviceContext->OMSetRenderTargets(1, renderTargets, m_depthStencilView.Get());
	
	const float clearColor[4] = { red, green, blue, alpha };
	
	m_deviceContext->ClearRenderTargetView(m_renderTargetView.Get(), clearColor);
	m_deviceContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
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
	DestroyDepthBuffer();

	m_transformBuffer.Reset();
	m_inputLayout.Reset();

	m_pixelShader.Reset();
	m_vertexShader.Reset();

	m_rasterizerState.Reset();

	m_swapChain.Reset();
	m_deviceContext.Reset();
	m_device.Reset();
}
