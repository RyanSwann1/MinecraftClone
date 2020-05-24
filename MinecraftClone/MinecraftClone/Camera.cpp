#include "Camera.h"

Camera::Camera(glm::vec3 startPosition)
	: m_speed(40.0f),
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

void Camera::mouse_callback(sf::Window& window)
{
	//float xoffset = xpos - lastX;
	//float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
	//lastX = xpos;
	//lastY = ypos;

	//float sensitivity = 0.3f; // change this value to your liking
	//xoffset *= sensitivity;
	//yoffset *= sensitivity;

	yaw += (sf::Mouse::getPosition(window).x - static_cast<int>(window.getSize().x / 2)) * 0.1f;
	pitch += (static_cast<int>(window.getSize().y / 2) - sf::Mouse::getPosition(window).y) * 0.1f;
	sf::Mouse::setPosition(sf::Vector2i(window.getSize().x / 2, window.getSize().y / 2), window);
	//yaw += xoffset;
	//pitch += yoffset;

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