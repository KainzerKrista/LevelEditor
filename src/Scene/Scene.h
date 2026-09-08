#pragma once

#include "Scene/Entity.h"

#include <cstdint>
#include <string>
#include <vector>

class Scene
{
public:
	Entity& CreateEntity(const std::string& name);
	Entity& CreateEntityWithID(std::uint32_t id, const std::string& name);

	Entity* FindEntity(std::uint32_t id);
	const Entity* FindEntity(std::uint32_t id) const;

	std::vector<Entity>& GetEntities();
	const std::vector<Entity>& GetEntities() const;

	bool DestroyEntity(std::uint32_t id);

	void Clear();
private:
	std::vector<Entity> m_entities;
	std::uint32_t m_nextEntityID = 1;
};