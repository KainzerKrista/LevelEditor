#pragma once

#include "Scene/Components/TransformComponent.h"
#include "Scene/Components/MeshComponent.h"

#include <cstdint>
#include <optional>
#include <string>

class Entity
{
public:
	Entity(std::uint32_t id, const std::string& name);

	std::uint32_t GetID() const;

	const std::string& GetName() const;
	
	void SetName(const std::string& name);

	TransformComponent& GetTransform();
	const TransformComponent& GetTransform() const;

	// Mesh Component
	bool HasMeshComponent() const;

	MeshComponent& AddMeshComponent();
	MeshComponent* GetMeshComponent();

	const MeshComponent* GetMeshComponent() const;

	void RemoveMeshComponent();

private:
	std::uint32_t m_id = 0;
	std::string m_name;
	TransformComponent m_transform;

	// Not all entities have a mesh component (e.g. Empty Entities or Lights)
	std::optional<MeshComponent> m_meshComponent;
};