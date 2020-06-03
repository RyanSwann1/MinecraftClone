#include "Player.h"
#include <SFML/Graphics.hpp>
#include "ChunkManager.h"
#include <cmath>

namespace 
{
	constexpr glm::vec3 STARTING_PLAYER_POSITION{ 0.0f, 100.0f, 0.0f };
	constexpr float WALKING_MOVEMENT_SPEED = 0.75f;
	constexpr float FLYING_MOVEMENT_SPEED = 5.0f;
	constexpr glm::vec3 MAX_VELOCITY = { 50.f, 50.0f, 50.0 };
	constexpr float VELOCITY_DROPOFF = 0.9f;
	constexpr float GRAVITY_AMOUNT = 0.025f;
	constexpr float HEAD_HEIGHT = 1.5f;
}

//Camera
Camera::Camera()
	: FOV(50.0f),
	sensitivity(0.1f),
	nearPlaneDistance(0.1f),
	farPlaneDistance(1750.f),
	front(),
	right(),
	up(),
	rotation(0.f, -90.f)
{
	front = { 0.0f, -1.0f, 0.0f };
	right = glm::normalize(glm::cross({ 0.0f, 1.0f, 0.0f }, front));
	up = { 0.0f, 1.0f, 0.0f };
}

void Camera::move(const sf::Window& window)
{
	rotation.x += (static_cast<int>(window.getSize().y / 2) - sf::Mouse::getPosition(window).y) * sensitivity;
	rotation.y += (sf::Mouse::getPosition(window).x - static_cast<int>(window.getSize().x / 2)) * sensitivity;

	
	//Reset mouse position to centre of screen
	sf::Mouse::setPosition(sf::Vector2i(window.getSize().x / 2, window.getSize().y / 2), window);

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (rotation.x > 89.0f)
		rotation.x = 89.0f;
	if (rotation.x < -89.0f)
		rotation.x = -89.0f;

	glm::vec3 v = {
		glm::cos(glm::radians(rotation.y)) * glm::cos(glm::radians(rotation.x)),
		glm::sin(glm::radians(rotation.x)),
		glm::sin(glm::radians(rotation.y)) * glm::cos(glm::radians(rotation.x)) };
	front = glm::normalize(v);
}

//Player
Player::Player()
	: m_camera(),
	m_position(STARTING_PLAYER_POSITION),
	m_velocity(),
	m_flying(false),
	m_applyGravity(true)
{}

const glm::vec3& Player::getPosition() const
{
	return m_position;
}

const Camera& Player::getCamera() const
{
	return m_camera;
}

void Player::toggleFlying()
{
	m_flying = !m_flying;
	m_applyGravity = !m_applyGravity;
}

void Player::reset()
{
	m_position = STARTING_PLAYER_POSITION;
}

void Player::moveCamera(const sf::Window& window)
{
	m_camera.move(window);
}

void Player::update(float deltaTime, std::mutex& playerMutex, const ChunkManager& chunkManager)
{
	move(deltaTime);
	
	std::lock_guard<std::mutex> playerLock(playerMutex);
	handleCollisions(chunkManager);
	m_position += m_velocity;
	m_velocity *= VELOCITY_DROPOFF;
}

void Player::move(float deltaTime)
{
	float movementSpeed = 0.0f;
	(m_flying ? movementSpeed = FLYING_MOVEMENT_SPEED : movementSpeed = WALKING_MOVEMENT_SPEED);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
	{
		m_velocity.x += glm::cos(glm::radians(m_camera.rotation.y)) * movementSpeed * deltaTime;
		m_velocity.z += glm::sin(glm::radians(m_camera.rotation.y)) * movementSpeed * deltaTime;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
	{
		m_velocity.x -= glm::cos(glm::radians(m_camera.rotation.y)) * movementSpeed * deltaTime;
		m_velocity.z -= glm::sin(glm::radians(m_camera.rotation.y)) * movementSpeed * deltaTime;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		m_velocity.x += glm::cos(glm::radians(m_camera.rotation.y + 90)) * movementSpeed * deltaTime;
		m_velocity.z += glm::sin(glm::radians(m_camera.rotation.y + 90)) * movementSpeed * deltaTime;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		m_velocity.x += glm::cos(glm::radians(m_camera.rotation.y - 90)) * movementSpeed * deltaTime;
		m_velocity.z += glm::sin(glm::radians(m_camera.rotation.y - 90)) * movementSpeed * deltaTime;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && m_flying)
	{
		m_velocity.y += movementSpeed * deltaTime;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && m_flying)
	{
		m_velocity.y -= movementSpeed * deltaTime;
	}

	if (m_applyGravity && !m_flying)
	{
		m_velocity.y -= GRAVITY_AMOUNT;
	}
}

//https://sites.google.com/site/letsmakeavoxelengine/home/collision-detection
void Player::handleCollisions(const ChunkManager& chunkManager)
{
	if (m_flying)
	{
		if (chunkManager.isCubeAtPosition({ std::floor(m_position.x), std::floor(m_position.y - HEAD_HEIGHT + m_velocity.y), std::floor(m_position.z) }) &&
			!sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			m_velocity.y = 0;
		}
	}
	else
	{
		if (chunkManager.isCubeAtPosition({ std::floor(m_position.x), std::floor(m_position.y - HEAD_HEIGHT - GRAVITY_AMOUNT), std::floor(m_position.z) }))
		{
			m_applyGravity = false;
			m_velocity.y = 0;
		}
		else
		{
			m_applyGravity = true;
		}
	}

	if (m_velocity.x > 0 &&
		chunkManager.isCubeAtPosition({ std::floor(m_position.x + WALKING_MOVEMENT_SPEED), std::floor(m_position.y - 0.75f), std::floor(m_position.z) }))
	{
		m_position.y += 2;
		m_position.x += 0.5f;
	}
	else if (m_velocity.x < 0 &&
		chunkManager.isCubeAtPosition({ std::floor(m_position.x - WALKING_MOVEMENT_SPEED), std::floor(m_position.y - 0.75f), std::floor(m_position.z) }))
	{
		m_position.y += 2;
		m_position.x -= 0.5f;
	}

	if (m_velocity.z > 0 &&
		chunkManager.isCubeAtPosition({ std::floor(m_position.x), std::floor(m_position.y - 0.75f), std::floor(m_position.z + WALKING_MOVEMENT_SPEED) }))
	{
		m_position.y += 2;
		m_position.z += 0.5f;
	}
	else if (m_velocity.z < 0 &&
		chunkManager.isCubeAtPosition({ std::floor(m_position.x), std::floor(m_position.y - 0.75f), std::floor(m_position.z - WALKING_MOVEMENT_SPEED) }))
	{
		m_position.y += 2;
		m_position.z -= 0.5f;
	}
}