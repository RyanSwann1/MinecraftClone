#include "Camera.h"

Camera::Camera(glm::vec3 startPosition)
	: m_speed(250.0f),
	m_position(startPosition),
	m_front(0.0f, 0.0f, -1.0f),
	m_up(0.0f, 1.0f, 0.0f)
{}

void Camera::move(float deltaTime)
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		m_position -= glm::normalize(glm::cross(m_front, m_up)) * m_speed * deltaTime;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		m_position += glm::normalize(glm::cross(m_front, m_up)) * m_speed * deltaTime;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
	{
		m_position += m_speed * m_front * deltaTime;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
	{
		m_position -= m_speed * m_front * deltaTime;
	}
}

void Camera::mouse_callback(double xpos, double ypos)
{
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f; // change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	m_front = glm::normalize(front);
}