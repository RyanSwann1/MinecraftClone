#include "Player.h"
#include <SFML/Graphics.hpp>

namespace 
{
	constexpr glm::vec3 STARTING_PLAYER_POSITION{ 0.0f, 100.0f, 0.0f };
	constexpr float WALKING_MOVEMENT_SPEED = 50.0f;
}

//Camera
Camera::Camera()
	: FOV(45.0f),
	sensitivity(0.1f),
	nearPlaneDistance(0.1f),
	farPlaneDistance(1750.f),
	front(),
	right(),
	up(),
	yaw(-90.0f),
	pitch(0.0f)
{
	front = { 0.0f, -1.0f, 0.0f };
	right = glm::normalize(glm::cross({ 0.0f, 1.0f, 0.0f }, front));
	up = { 0.0f, 1.0f, 0.0f };
}

void Camera::move(const sf::Window& window)
{
	yaw += (sf::Mouse::getPosition(window).x - static_cast<int>(window.getSize().x / 2)) * sensitivity;
	pitch += (static_cast<int>(window.getSize().y / 2) - sf::Mouse::getPosition(window).y) * sensitivity;
	
	//Reset mouse position to centre of screen
	sf::Mouse::setPosition(sf::Vector2i(window.getSize().x / 2, window.getSize().y / 2), window);

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 v = {
		glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch)),
		glm::sin(glm::radians(pitch)),
		glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch)) };
	front = glm::normalize(v);
}

//Player
Player::Player()
	: m_camera(),
	m_position(STARTING_PLAYER_POSITION),
	m_movementSpeed(WALKING_MOVEMENT_SPEED)
{}

const glm::vec3& Player::getPosition() const
{
	return m_position;
}

const Camera& Player::getCamera() const
{
	return m_camera;
}

void Player::reset(glm::vec3& playerPositionOnChunkGeneration)
{
	m_position = STARTING_PLAYER_POSITION;
	playerPositionOnChunkGeneration = m_position;
}

void Player::moveCamera(const sf::Window& window)
{
	m_camera.move(window);
}

void Player::move(float deltaTime, std::mutex& playerMutex, glm::vec3& playerPositionOnChunkGeneration)
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
		sf::Keyboard::isKeyPressed(sf::Keyboard::S) ||
		sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
		sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		{
			m_position -= glm::normalize(glm::cross(m_camera.front, m_camera.up)) * m_movementSpeed * deltaTime;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		{
			m_position += glm::normalize(glm::cross(m_camera.front, m_camera.up)) * m_movementSpeed * deltaTime;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		{
			m_position += m_movementSpeed * m_camera.front * deltaTime;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		{
			m_position -= m_movementSpeed * m_camera.front * deltaTime;
		}

		if (playerMutex.try_lock())
		{
			playerPositionOnChunkGeneration = m_position;
			playerMutex.unlock();
		}
	}
}