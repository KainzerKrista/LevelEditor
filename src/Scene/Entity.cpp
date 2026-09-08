#include "Scene/Entity.h"

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

bool Entity::HasMeshComponent() const
{
	return m_meshComponent.has_value();
}

MeshComponent& Entity::AddMeshComponent()
{
	// Constructs a mesh component inside an entity
	if (!m_meshComponent.has_value())
	{
		m_meshComponent.emplace();
	}

	return m_meshComponent.value();
}

MeshComponent* Entity::GetMeshComponent()
{
	if (!m_meshComponent.has_value())
	{
		return nullptr;
	}

	return &m_meshComponent.value();
}

const MeshComponent* Entity::GetMeshComponent() const
{
	if (!m_meshComponent.has_value())
	{
		return nullptr;
	}

	return &m_meshComponent.value();
}

void Entity::RemoveMeshComponent()
{
	// Removes mesh component inside an entity
	m_meshComponent.reset();
}