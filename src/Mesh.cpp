#include "Mesh.h"

#include <iostream>

bool Mesh::Initialize(ID3D11Device* device, std::span<const Vertex> vertices, std::span<const std::uint16_t> indices)
{
	if (device == nullptr)
	{
		std::cerr << "ERROR: Mesh cannot be created without a D3D11 device\n";
		return false;
	}

	if (indices.empty())
	{
		std::cerr << "ERROR: Mesh cannot be created without indicies\n";
		return false;
	}

	// Vertex Buffer
	// Create Vertex Buffer
	D3D11_BUFFER_DESC bufferDescription{};

	bufferDescription.ByteWidth = static_cast<UINT>(vertices.size_bytes());
	bufferDescription.Usage = D3D11_USAGE_IMMUTABLE; // Vertex of object won't move
	bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vertexData{};

	vertexData.pSysMem = vertices.data();

	HRESULT result = device->CreateBuffer(&bufferDescription, &vertexData, &m_vertexBuffer);

	if (FAILED(result))
	{
		std::cerr << "ERROR: Failed to create vertex buffer\n";
		return false;
	}

	std::cout << "Vertex Buffer created successfully\n";

	// Create Index Buffer
	D3D11_BUFFER_DESC indexBufferDescription{};

	indexBufferDescription.ByteWidth = static_cast<UINT>(indices.size_bytes());
	indexBufferDescription.Usage = D3D11_USAGE_IMMUTABLE;  // Vertex of object won't move
	indexBufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;

	// Create Index Data
	D3D11_SUBRESOURCE_DATA indexData{};
	indexData.pSysMem = indices.data();

	result = device->CreateBuffer(&indexBufferDescription, &indexData, &m_indexBuffer);

	if (FAILED(result))
	{
		std::cerr << "ERROR: Failed to create Index Buffer\n";
		return false;
	}

	std::cerr << "Index Buffer created successfully\n";

	m_indexCount = static_cast<UINT>(indices.size());

	return true;
}

void Mesh::Shutdown()
{
	m_indexBuffer.Reset();
	m_vertexBuffer.Reset();

	m_indexCount = 0;
}

ID3D11Buffer* Mesh::GetVertexBuffer() const
{
	return m_vertexBuffer.Get();
}

ID3D11Buffer* Mesh::GetIndexBuffer() const
{
	return m_indexBuffer.Get();
}

UINT Mesh::GetIndexCount() const
{
	return m_indexCount;
}