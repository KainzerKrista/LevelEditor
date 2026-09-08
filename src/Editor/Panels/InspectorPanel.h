#pragma once

#include "Assets/AssetManager.h"
#include "Scene/Scene.h"

#include <cstdint>

class InspectorPanel
{
public: 
	void Draw(Scene& scene, AssetManager& assetManager, std::uint32_t& selectedEntityID);
};