#include "EditorCamera.h"

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

EditorCamera::EditorCamera()
{

}

void EditorCamera::Move(float forwardInput, float rightInput, float upInput, float deltaTime, bool boost)
{
	glm::vec3 direction(0.0f);

	const glm::vec3 forward = GetForwardVector();
	const glm::vec3 right = GetRightVector();
	const glm::vec3 worldUp(0.0f, 1.0f, 0.0f);

	direction += forward * forwardInput;
	direction += right * rightInput;
	direction += worldUp * upInput;

	// Avoid normalising zero length vectors
	if (glm::dot(direction, direction) > 0.0f)
	{
		direction = glm::normalize(direction);
	}

	float speed = m_moveSpeed;

	if (boost)
	{
		speed *= m_boostMultiplier;
	}

	m_position += direction * speed * deltaTime;
}

void EditorCamera::Rotate(float deltaX, float deltaY)
{
	// Mouse Look
	m_yaw += deltaX * m_mouseSensitivity;

	// Invert camera pitch while Mouse Y increases from moving downwards
	m_pitch -= deltaY * m_mouseSensitivity;

	// Stops camera from flipping upsidedown
	m_pitch = glm::clamp(m_pitch, -89.0f, 89.0f);
}

void EditorCamera::SetViewportSize(int width, int height)
{
	if (width <= 0 || height <= 0)
	{
		return;
	}

	m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);
}

// Camera States
void EditorCamera::SetLooking(bool looking)
{
	m_isLooking = looking;
}

bool EditorCamera::IsLooking() const
{
	return m_isLooking;
}

// Camera Matrices
glm::mat4 EditorCamera::GetViewMatrix() const
{
	const glm::vec forward = GetForwardVector();

	return glm::lookAtLH(m_position, m_position + forward, glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::mat4 EditorCamera::GetProjectionMatrix() const
{
	return glm::perspectiveLH_ZO(glm::radians(m_fieldOfView), m_aspectRatio, m_nearPlane, m_farPlane);
}

const glm::vec3& EditorCamera::GetPosition() const
{
	return m_position;
}

// Vector Directions
glm::vec3 EditorCamera::GetForwardVector() const
{
	const float yaw = glm::radians(m_yaw);
	const float pitch = glm::radians(m_pitch);

	glm::vec3 forward;
	
	forward.x = glm::cos(pitch) * glm::sin(yaw);
	forward.y = glm::sin(pitch);
	forward.z = glm::cos(pitch) * glm::cos(yaw);

	return glm::normalize(forward);
}

glm::vec3 EditorCamera::GetRightVector() const
{
	const glm::vec3 worldUp(0.0f, 1.0f, 0.0f);

	return glm::normalize(glm::cross(worldUp, GetForwardVector()));
}

