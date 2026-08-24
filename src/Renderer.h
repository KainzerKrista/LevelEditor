#pragma once
#include "Mesh.h"
#include <Windows.h>

// Direct3D 11
#include <d3d11.h>

// Swap Chain
#include <dxgi.h>


// Microsoft's ComPtr to handle D3D objects
#include <wrl/client.h>

#include <glm/glm.hpp>
#include<cstdint>
#include<span>

class Renderer
{
public:
	bool Initialize(HWND windowHandle, int width, int height);

	// Render Lifecycle
	void Shutdown();
	void Resize(int width, int height);
	void BeginFrame(float red, float green, float blue, float alpha);
	void EndFrame();

	bool CreateMesh(Mesh& mesh, std::span<const Vertex> vertices, std::span<const std::uint16_t> indices);
	void DrawMesh(const Mesh& mesh, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection);

	bool ResizeViewport(int width, int height);
	void BeginViewportFrame(float red, float green, float blue, float alpha);

	void EndViewportFrame();

	ID3D11Device* GetDevice() const;
	ID3D11DeviceContext* GetDeviceContext() const;

	ID3D11ShaderResourceView* GetViewportShaderResourceView() const;

private:
	bool CreateRenderTarget();
	void DestroyRenderTarget();


	bool CreateDepthBuffer(int width, int height);
	void DestroyDepthBuffer();

	bool CreateMeshPipeline();

	bool CreateViewportRenderTarget(int width, int height);
	void DestroyViewportRenderTarget();

	void UpdateViewport(int width, int height);

private:
	Microsoft::WRL::ComPtr<ID3D11Device> m_device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_deviceContext;

	Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

	Microsoft::WRL::ComPtr<ID3D11Buffer> m_transformBuffer;

	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rasterizerState;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_viewportTexture;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_viewportRenderTargetView;
	Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> m_viewportShaderResourceView;
	Microsoft::WRL::ComPtr <ID3D11Texture2D> m_viewportDepthBuffer;
	Microsoft::WRL::ComPtr <ID3D11DepthStencilView> m_viewportDepthStencilView;

	int m_viewportWidth = 0;
	int m_viewportHeight = 0;

	int m_width = 0;
	int m_height = 0;

};