#pragma once

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

struct TransformComponent
{
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 rotation = glm::vec3(0.0f);
	glm::vec3 scale = glm::vec3(1.0f);

	glm::mat4 GetTranform() const
	{
		glm::mat4 transform = glm::mat4(1.0f);
		
		// Position
		transform = glm::translate(transform, position);

		// Rotation X
		transform = glm::rotate(transform, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));

		// Rotation Y
		transform = glm::rotate(transform, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));

		// Rotaion Z
		transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

		// Scale
		transform = glm::scale(transform, scale);

		return transform;
	}
};