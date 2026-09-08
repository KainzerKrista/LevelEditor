#include "Scene/Scene.h"

Entity& Scene::CreateEntity(const std::string& name)
{
	// Ensures entities do not reuse IDs if one gets deleted
	const std::uint32_t id = m_nextEntityID++;

	m_entities.emplace_back(id, name);

	return m_entities.back();
}

Entity& Scene::CreateEntityWithID(std::uint32_t id, const std::string& name)
{
	m_entities.emplace_back(id, name);

	if (id >= m_nextEntityID)
	{
		m_nextEntityID = id + 1;
	}

	return m_entities.back();
}

Entity* Scene::FindEntity(std::uint32_t id)
{
	for (Entity& entity : m_entities)
	{
		if (entity.GetID() == id)
		{
			return &entity;
		}
	}

	return nullptr;
}

const Entity* Scene::FindEntity(std::uint32_t id) const
{
	for (const Entity& entity : m_entities)
	{
		if (entity.GetID() == id)
		{
			return &entity;
		}
	}

	return nullptr;
}

std::vector<Entity>& Scene::GetEntities()
{
	return m_entities;
}


const std::vector<Entity>& Scene::GetEntities() const
{
	return m_entities;
}

bool Scene::DestroyEntity(std::uint32_t id)
{
	// Iterates through list of scene entities and deletes the selected entity.
	for (auto iterator = m_entities.begin(); iterator != m_entities.end(); ++iterator)
	{
		if (iterator->GetID() == id)
		{
			m_entities.erase(iterator);
			return true;
		}
	}

	return false;
}

void Scene::Clear()
{
	m_entities.clear();
	m_nextEntityID = 1;
}