#include "Camera.h"

Camera::Camera()
	: m_speed(0.5f),
	m_position(0.0f, 0.0f, 3.0f),
	m_front(0.0f, 0.0f, -1.0f),
	m_frontInverse(0.0f, 0.0f, 1.0f),
	m_up(0.0f, 1.0f, 0.0f)
{}

void Camera::move(const sf::Event & sfmlEvent, float deltaTime)
{
	switch (sfmlEvent.key.code)
	{
	case sf::Keyboard::A:
		m_position -= glm::normalize(glm::cross(m_front, m_up)) * m_speed;
		break;
	case sf::Keyboard::D:
		m_position += glm::normalize(glm::cross(m_front, m_up)) * m_speed;
		break;
	case sf::Keyboard::W:
		m_position += m_speed * m_front;
		break;
	case sf::Keyboard::S:
		m_position -= m_speed * m_front;
		break;
	}
}

void Camera::mouse_callback(double xpos, double ypos)
{
	//if (firstMouse)
	//{
	//	lastX = xpos;
	//	lastY = ypos;
	//	firstMouse = false;
	//}

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
	
	

	//m_frontInverse.x = -glm::normalize(front).x * 5;
	//m_frontInverse.y = -glm::normalize(front).y * 5;
	//m_frontInverse.z = -glm::normalize(front).z * 5;

	m_frontInverse.x = -glm::normalize(front).x * 5;
	m_frontInverse.y = -glm::normalize(front).y * 5;
	m_frontInverse.z = -glm::normalize(front).z * 5;

	m_frontInverse.x *= 2;
	m_frontInverse.y *= 2;
	m_frontInverse.z *= 2;
}