#include "Assets/AssetManager.h"

#include <algorithm>

bool AssetManager::RegisterMesh(const std::string& assetName, std::unique_ptr<Mesh> mesh)
{
	
	if (assetName.empty() || mesh == nullptr)
	{
		return false;
	}

	// Ensures asset names are not duplicated
	if (m_meshes.contains(assetName))
	{
		return false;
	}

	m_meshes.emplace(assetName, std::move(mesh));

	return true;
}

Mesh* AssetManager::GetMesh(const std::string& assetName) const
{
	auto iterator = m_meshes.find(assetName);

	if (iterator == m_meshes.end())
	{
		return nullptr;
	}

	return iterator->second.get();
}

std::vector<std::string> AssetManager::GetMeshAssetNames() const
{
	std::vector<std::string> names;

	names.reserve(m_meshes.size());

	for (const auto& [name, mesh] : m_meshes)
	{
		names.push_back(name);
	}
	
	// Sort asset browser alphabetically
	std::sort(names.begin(), names.end());

	return names;
}

void AssetManager::Shutdown()
{
	for (auto& [name, mesh] : m_meshes)
	{
		if (mesh != nullptr)
		{
			mesh->Shutdown();
		}
	}

	m_meshes.clear();
}

