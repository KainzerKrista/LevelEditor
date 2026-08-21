#pragma once

#include <d3d11.h>
#include <wrl/client.h>

#include <cstdint>
#include <span>

// match semantics in HLSL
struct Vertex
{
	// x, y, z
	float position[3];

	// red, green blue, alpha
	float color[4];
};

class Mesh
{
public:
	bool Initialize(ID3D11Device* device, std::span<const Vertex> vertices, std::span<const std::uint16_t> indices);

	void Shutdown();

	ID3D11Buffer* GetVertexBuffer() const;
	ID3D11Buffer* GetIndexBuffer() const;

	UINT GetIndexCount() const;

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;

	UINT m_indexCount = 0;
};