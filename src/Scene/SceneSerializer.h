#pragma once

#include "Scene/Scene.h"

#include <functional>
#include <string>

class SceneSerializer
{
public:
	using MeshResolver = std::function<Mesh* (const std::string&)>;

	static bool Save(const Scene& scene, const std::string& filePath);

	static bool Load(Scene& scene, const std::string& filePath, const MeshResolver& meshResolver);
};