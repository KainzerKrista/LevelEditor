#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Renderer/Mesh.h"

class AssetManager
{
public:
	bool RegisterMesh(const std::string& assetName, std::unique_ptr<Mesh> mesh);

	Mesh* GetMesh(const std::string& assetName) const;
	std::vector<std::string> GetMeshAssetNames() const;

	void Shutdown();

private: 
	std::unordered_map<std::string,std::unique_ptr<Mesh> > m_meshes;
};