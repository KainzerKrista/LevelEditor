#pragma once

#include "Entity.h"

#include <cstdint>
#include <string>
#include <vector>

class Scene
{
public:
	Entity& CreateEntity(const std::string& name);
	Entity* FindEntity(std::uint32_t id);
	const Entity* FindEntity(std::uint32_t id) const;

	std::vector<Entity>& GetEntities();
	const std::vector<Entity>& GetEntities() const;

private:
	std::vector<Entity> m_entities;
	std::uint32_t m_nextEntityID = 1;
};