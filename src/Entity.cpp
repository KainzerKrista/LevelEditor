#include "Entity.h"

Entity::Entity(std::uint32_t id, const std::string& name) : m_id(id), m_name(name)
{

}

std::uint32_t Entity::GetID() const
{
	return m_id;
}

const std::string& Entity::GetName() const
{
	return m_name;
}

void Entity::SetName(const std::string& name)
{
	m_name = name;
}

TransformComponent& Entity::GetTransform()
{
	return m_transform;
}

const TransformComponent& Entity::GetTransform() const
{
	return m_transform;
}

void Entity::SetMesh(Mesh* mesh)
{
	m_mesh = mesh;
}

Mesh* Entity::GetMesh() const
{
	return m_mesh;
}