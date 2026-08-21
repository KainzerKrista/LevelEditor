#pragma once

#include <glm/glm.hpp>

class EditorCamera
{
public:
		EditorCamera();

		void Move(float forwardInput, float rightInput, float upInput, float deltaTime, bool boost);
		void Rotate(float deltaX, float deltaY);
		void SetViewportSize(int width, int height);

		// Right-Mouse Camera Control
		void SetLooking(bool lookling);
		bool IsLooking() const;

		glm::mat4 GetViewMatrix() const;
		glm::mat4 GetProjectionMatrix() const;

		const glm::vec3& GetPosition() const;

private:
		glm::vec3 GetForwardVector() const;
		glm::vec3 GetRightVector() const;

private:
		glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);

		float m_yaw = 0.0f;
		float m_pitch = 0.0f;

		float m_moveSpeed = 3.0f;
		float m_boostMultiplier = 3.0f;

		float m_mouseSensitivity = 0.1f;

		float m_fieldOfView = 60.0f;
		float m_nearPlane = 0.1f;
		float m_farPlane = 1000.0f;

		float m_aspectRatio = 16.0f / 9.0f;

		bool m_isLooking = false;


};