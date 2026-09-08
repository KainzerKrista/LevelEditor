#pragma once

#include "Scene/Scene.h"

#include <cstdint>

class HierarchyPanel
{
public:
	void Draw(Scene& scene, std::uint32_t& selectedEntityID);
};