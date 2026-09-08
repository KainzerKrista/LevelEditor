// Handles Mesh Data on CPU away from mesh buffers on cpu

#pragma once

#include "Renderer/Mesh.h"

#include <cstdint>
#include <vector>

struct MeshData
{
	std::vector<Vertex> vertices;
	std::vector<std::uint32_t> indices;
};

