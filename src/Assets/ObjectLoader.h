#pragma once

#include "Renderer/MeshData.h"

#include <string>

class ObjectLoader
{
public: 
	static bool Load(const std::string& filePath, MeshData& meshData);
};