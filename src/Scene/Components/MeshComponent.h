#pragma once 

#include "Renderer/Mesh.h"

#include <string>

struct MeshComponent
{
	// Save to scene file
	std::string assetName;

	// Runtime only
	Mesh* mesh = nullptr;
};