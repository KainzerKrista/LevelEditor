#pragma once

#include "TransformComponent.h"
#include "Mesh.h"

#include <cstdint>
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

	void SetMesh(Mesh* mesh);
	Mesh* GetMesh() const;

private:
	std::uint32_t m_id = 0;
	std::string m_name;
	TransformComponent m_transform;
	Mesh* m_mesh = nullptr;

};