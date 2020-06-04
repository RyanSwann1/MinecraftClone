#include "Player.h"
#include <SFML/Graphics.hpp>
#include "ChunkManager.h"
#include <cmath>

namespace 
{
	constexpr glm::vec3 STARTING_PLAYER_POSITION{ 0.0f, 100.0f, 0.0f };
	constexpr float WALKING_MOVEMENT_SPEED = 0.55f;
	constexpr float FLYING_MOVEMENT_SPEED = 5.0f;
	constexpr glm::vec3 MAX_VELOCITY = { 50.f, 50.0f, 50.0 };
	constexpr float VELOCITY_DROPOFF = 0.9f;
	constexpr float GRAVITY_AMOUNT = 1.0f;
	constexpr float HEAD_HEIGHT = 2.25f;
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
	m_applyGravity(true),
	m_onGround(false),
	m_jumping(false)
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

	if (m_flying)
	{
		m_onGround = false;
		m_jumping = false;
	}
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
	
	m_position += m_velocity * deltaTime;

	if (m_flying)
	{
		m_velocity *= VELOCITY_DROPOFF;
	}
	else
	{
		m_velocity.x *= VELOCITY_DROPOFF;
		m_velocity.z *= VELOCITY_DROPOFF;
	}
}

void Player::move(float deltaTime)
{
	float movementSpeed = (m_flying ? FLYING_MOVEMENT_SPEED : WALKING_MOVEMENT_SPEED);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
	{
		m_velocity.x += glm::cos(glm::radians(m_camera.rotation.y)) * movementSpeed;
		m_velocity.z += glm::sin(glm::radians(m_camera.rotation.y)) * movementSpeed;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
	{
		m_velocity.x -= glm::cos(glm::radians(m_camera.rotation.y)) * movementSpeed;
		m_velocity.z -= glm::sin(glm::radians(m_camera.rotation.y)) * movementSpeed;
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
	{
		m_velocity.x += glm::cos(glm::radians(m_camera.rotation.y + 90)) * movementSpeed;
		m_velocity.z += glm::sin(glm::radians(m_camera.rotation.y + 90)) * movementSpeed;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
	{
		m_velocity.x += glm::cos(glm::radians(m_camera.rotation.y - 90)) * movementSpeed;
		m_velocity.z += glm::sin(glm::radians(m_camera.rotation.y - 90)) * movementSpeed;
	}
	if (m_flying)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && m_flying)
		{
			m_velocity.y += movementSpeed;
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && m_flying && !m_onGround)
		{
			m_velocity.y -= movementSpeed;
		}
	}
	else
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && m_onGround)
		{
			m_velocity.y += 3.0f;
			m_jumping = true;
		}

		if (m_applyGravity && !m_onGround)
		{
			m_velocity.y -= GRAVITY_AMOUNT;
		}
	}
}

//https://sites.google.com/site/letsmakeavoxelengine/home/collision-detection
void Player::handleCollisions(const ChunkManager& chunkManager)
{
	if (m_flying)
	{
		if (!m_onGround && 
			chunkManager.isCubeAtPosition({ std::floor(m_position.x), std::floor(m_position.y - HEAD_HEIGHT), std::floor(m_position.z) }) &&
			!sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			m_velocity.y = 0;
			m_position.y += std::abs(m_position.y - HEAD_HEIGHT - (std::floor(m_position.y - HEAD_HEIGHT) + 1));
			m_position.y = std::floor(m_position.y);
			m_onGround = true;
		}
		else if(!chunkManager.isCubeAtPosition({ std::floor(m_position.x), std::floor(m_position.y - HEAD_HEIGHT), std::floor(m_position.z) }))
		{
			m_onGround = false;
		}
	}
	else
	{
		if (!m_onGround &&
			chunkManager.isCubeAtPosition({ std::floor(m_position.x), std::floor(m_position.y - HEAD_HEIGHT), std::floor(m_position.z) }))
		{
			m_velocity.y = 0;
			m_position.y += std::abs(m_position.y - HEAD_HEIGHT - (std::floor(m_position.y - HEAD_HEIGHT) + 1));
			m_position.y = std::floor(m_position.y);
			m_onGround = true;
			m_jumping = false;
		}
		else if (!chunkManager.isCubeAtPosition({ std::floor(m_position.x), std::floor(m_position.y - HEAD_HEIGHT), std::floor(m_position.z) }))
		{
			m_onGround = false;
			
		}
	}

	float liftSpeed = 3.5f;
	float brakeAmount = 2.5f;
	if (!m_jumping)
	{
		if (m_velocity.x > 0 &&
			chunkManager.isCubeAtPosition({ std::floor(m_position.x + WALKING_MOVEMENT_SPEED), std::floor(m_position.y - 2.0f), std::floor(m_position.z) }))
		{
			m_velocity.y += liftSpeed;
			m_velocity.x -= brakeAmount;
		}
		else if (m_velocity.x < 0 &&
			chunkManager.isCubeAtPosition({ std::floor(m_position.x - WALKING_MOVEMENT_SPEED), std::floor(m_position.y - 2.0f), std::floor(m_position.z) }))
		{
			m_velocity.y += liftSpeed;
			m_velocity.x += brakeAmount;
		}

		if (m_velocity.z > 0 &&
			chunkManager.isCubeAtPosition({ std::floor(m_position.x), std::floor(m_position.y - 2.0f), std::floor(m_position.z + WALKING_MOVEMENT_SPEED) }))
		{
			m_velocity.y += liftSpeed;
			m_velocity.z -= brakeAmount;
		}
		else if (m_velocity.z < 0 &&
			chunkManager.isCubeAtPosition({ std::floor(m_position.x), std::floor(m_position.y - 2.0f), std::floor(m_position.z - WALKING_MOVEMENT_SPEED) }))
		{
			m_velocity.y += liftSpeed;
			m_velocity.z += brakeAmount;
		}
	}	
}