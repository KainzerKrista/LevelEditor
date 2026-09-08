#include "Scene/SceneSerializer.h"

#include <nlohmann/json.hpp>

#include <fstream>
#include <iostream>

using json = nlohmann::json;

bool SceneSerializer::Save(const Scene& scene, const std::string& filePath)
{
	json sceneJson;

	sceneJson["scene"] = "Untitled";
	sceneJson["entity"] = json::array();

	for (const Entity& entity : scene.GetEntities())
	{
		json entityJson;

		// Entity Information
		entityJson["id"] = entity.GetID();
		entityJson["name"] = entity.GetName();

		// Entity Transform
		const TransformComponent& transform = entity.GetTransform();

		entityJson["transform"]["position"] = { transform.position.x, transform.position.y, transform.position.z };
		entityJson["transform"]["rotation"] = { transform.position.x, transform.position.y, transform.position.z };
		entityJson["transform"]["scale"] = { transform.position.x, transform.position.y, transform.position.z };

		// Entity Mesh 
		const MeshComponent* meshComponent = entity.GetMeshComponent();

		if (meshComponent != nullptr)
		{
			entityJson["mesh"]["asset"] = meshComponent->assetName;
		}

		sceneJson["entities"].push_back(entityJson);
	}

	// WWrite JSON to disk
	std::ofstream file(filePath);
	if (!file.is_open())
	{
		std::cerr
			<< "ERROR: Failed to save scene: "
			<< filePath
			<< '\n';

		return false;
	}

	file << sceneJson.dump(4);

	return true;
}

bool SceneSerializer::Load(Scene& scene, const std::string& filePath, const MeshResolver& meshResolver)
{
	std::ifstream file(filePath);
	if (!file.is_open())
	{
		std::cerr << "ERROR: Failed to open scene: "
			<< filePath
			<< '\n';

		return false;
	}

	json sceneJson;

	try
	{
		file >> sceneJson;
	}

	catch (const json::exception& exception)
	{
		std::cerr
			<< "ERROR: Invalid scene JSON: "
			<< exception.what()
			<< '\n';

		return false;
	}

	if (!sceneJson.contains("entities") || !sceneJson["entities"].is_array())
	{
		std::cerr << "ERROR: Scene does not contain a valid entities array\n";

		return false;
	}

	for (const json& entityJson : sceneJson["entities"])
	{
		const std::uint32_t id = entityJson.value("id", 0u);
		const std::string name = entityJson.value("name", "Entity");

		if (id == 0)
		{
			continue;
		}

		Entity& entity = scene.CreateEntityWithID(id, name);

		// Transform
		if (entityJson.contains("transform"))
		{
			const json& transformJson = entityJson["transform"];

			TransformComponent& transform = entity.GetTransform();

			if (transformJson.contains("position"))
			{
				const json& value = transformJson["position"];

				if (value.is_array() && value.size() == 3)
				{
					transform.position = glm::vec3(value[0].get<float>(), value[1].get<float>(), value[2].get<float>());
				}
			}

			if (transformJson.contains("rotation"))
			{
				const json& value = transformJson["rotation"];

				if (value.is_array() && value.size() == 3)
				{
					transform.rotation = glm::vec3(value[0].get<float>(), value[1].get<float>(), value[2].get<float>());
				}
			}

			if (transformJson.contains("scale"))
			{
				const json& value = transformJson["scale"];

				if (value.is_array() && value.size() == 3)
				{
					transform.scale = glm::vec3(value[0].get<float>(), value[1].get<float>(), value[2].get<float>());
				}
			}
		}

		// Mesh
		if (entityJson.contains("mesh"))
		{
			const std::string assetName = entityJson["mesh"].value("asset", "");

			MeshComponent& meshComponent = entity.AddMeshComponent();
			meshComponent.assetName = assetName;
			meshComponent.mesh = meshResolver(assetName);
		}
	}

	return true;
}