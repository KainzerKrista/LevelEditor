#include "Scene.h"

Entity& Scene::CreateEntity(const std::string& name)
{
	const std::uint32_t id = m_nextEntityID++;

	m_entities.emplace_back(id, name);

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