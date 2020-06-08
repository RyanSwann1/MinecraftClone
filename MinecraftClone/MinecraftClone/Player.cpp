#include "Player.h"
#include <SFML/Graphics.hpp>
#include "ChunkManager.h"
#include <cmath>
#include <iostream>

namespace 
{
	constexpr float WALKING_MOVEMENT_SPEED = 0.55f;
	constexpr float FLYING_MOVEMENT_SPEED = 5.0f;
	constexpr glm::vec3 MAX_VELOCITY = { 50.f, 50.0f, 50.0 };
	constexpr float JUMP_SPEED = 12.0f;

	constexpr float VELOCITY_DROPOFF = 0.9f;
	constexpr float GRAVITY_AMOUNT = 1.0f;
	constexpr float HEAD_HEIGHT = 2.25f;
	constexpr int MS_BETWEEN_ATTEMPT_SPAWN = 250;
	
	constexpr float AUTO_JUMP_DISTANCE = WALKING_MOVEMENT_SPEED * 1.5f;
	constexpr float AUTO_JUMP_HEIGHT = 2.0f;
	constexpr float AUTO_JUMP_BREAK_SCALAR = 0.2f;

	constexpr float DESTROY_BLOCK_RANGE = 7.5f;

	const CubeTypeComparison NON_COLLIDABLE_CUBE_TYPES =
	{
		{eCubeType::Water,
		eCubeType::TallGrass,
		eCubeType::Shrub}
	};
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
	m_currentState(ePlayerState::InAir),
	m_position(),
	m_velocity()
{}

const glm::vec3& Player::getPosition() const
{
	return m_position;
}

const Camera& Player::getCamera() const
{
	return m_camera;
}

void Player::destroyFacingBlock(ChunkManager& chunkManager, std::mutex& playerMutex)
{
	std::lock_guard<std::mutex> playerLock(playerMutex);
	for (float i = 0; i <= DESTROY_BLOCK_RANGE; i += 0.5f)
	{
		glm::vec3 rayPosition = m_camera.front * i + m_position;
		chunkManager.destroyCubeAtPosition({ std::floor(rayPosition.x), std::floor(rayPosition.y), std::floor(rayPosition.z) });
	}
}

void Player::spawn(const ChunkManager& chunkManager, std::mutex& playerMutex)
{
	bool spawned = false;
	while (!spawned)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(MS_BETWEEN_ATTEMPT_SPAWN));

		std::lock_guard<std::mutex> playerLock(playerMutex);
		if (chunkManager.isChunkAtPosition(Utilities::PLAYER_STARTING_POSITION))
		{
			m_position = chunkManager.getHighestCubeAtPosition(Utilities::PLAYER_STARTING_POSITION);
			m_position.y += HEAD_HEIGHT;
			m_velocity = glm::vec3();
			m_currentState = ePlayerState::InAir;
			spawned = true;
		}
	}
}

void Player::toggleFlying()
{
	switch (m_currentState)
	{
	case ePlayerState::InAir:
		m_currentState = ePlayerState::Flying;
		break;
	case ePlayerState::Flying:
		m_currentState = ePlayerState::InAir;
		break;
	case ePlayerState::OnGround:
		break;
	default:
		assert(false);
	}
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

	switch (m_currentState)
	{
	case ePlayerState::Flying:
		m_velocity *= VELOCITY_DROPOFF;
		break;
	case ePlayerState::InAir:
	case ePlayerState::OnGround:
		m_velocity.x *= VELOCITY_DROPOFF;
		m_velocity.z *= VELOCITY_DROPOFF;
		break;
	default:
		assert(false);
	}
}

void Player::move(float deltaTime)
{
	float movementSpeed = (m_currentState == ePlayerState::Flying ? FLYING_MOVEMENT_SPEED : WALKING_MOVEMENT_SPEED);

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

	switch (m_currentState)
	{
	case ePlayerState::Flying:
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			m_velocity.y += movementSpeed;
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
		{
			m_velocity.y -= movementSpeed;
		}
		break;
	case ePlayerState::InAir :
		m_velocity.y -= GRAVITY_AMOUNT;
		break;
	case ePlayerState::OnGround:
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && m_velocity.y == 0)
		{
			m_velocity.y += JUMP_SPEED;
		}
		break;
	default:
		assert(false);
	}
}

//https://sites.google.com/site/letsmakeavoxelengine/home/collision-detection
void Player::handleCollisions(const ChunkManager& chunkManager)
{
	//Handle vertical collisions
	eCubeType cubeType = eCubeType::Air;
	if (m_currentState != ePlayerState::OnGround &&
		chunkManager.isCubeAtPosition({ std::floor(m_position.x), std::floor(m_position.y - HEAD_HEIGHT), std::floor(m_position.z) }, cubeType) &&
		!NON_COLLIDABLE_CUBE_TYPES.isMatch(cubeType))
	{
		m_velocity.y = 0;
		m_position.y += std::abs(m_position.y - HEAD_HEIGHT - (std::floor(m_position.y - HEAD_HEIGHT) + 1));
		m_position.y = std::floor(m_position.y);

		m_currentState = ePlayerState::OnGround;
	}
	else if (!chunkManager.isCubeAtPosition({ std::floor(m_position.x), std::floor(m_position.y - HEAD_HEIGHT), std::floor(m_position.z) }, cubeType) ||
		NON_COLLIDABLE_CUBE_TYPES.isMatch(cubeType))
	{
		m_currentState = (m_currentState != ePlayerState::Flying ? ePlayerState::InAir : m_currentState);
	}


	//Handle Auto Jump 
	if (m_currentState == ePlayerState::OnGround && m_velocity.y == 0)
	{
		for (int z = Utilities::CUBE_SIZE; z >= -Utilities::CUBE_SIZE; z -= Utilities::CUBE_SIZE * 2)
		{
			for (int x = -Utilities::CUBE_SIZE; x <= Utilities::CUBE_SIZE; x += Utilities::CUBE_SIZE * 2)
			{
				glm::vec3 collisionPosition(
					m_position.x + glm::cos((glm::radians(m_camera.rotation.y)) * (AUTO_JUMP_DISTANCE * x)),
					m_position.y,
					m_position.z + glm::sin((glm::radians(m_camera.rotation.y)) * (AUTO_JUMP_DISTANCE * z)));

				if (chunkManager.isCubeAtPosition({ std::floor(collisionPosition.x), std::floor(collisionPosition.y - AUTO_JUMP_HEIGHT), std::floor(collisionPosition.z) }, cubeType) &&
					!NON_COLLIDABLE_CUBE_TYPES.isMatch(cubeType))
				{
					m_velocity.y += JUMP_SPEED;
					m_velocity.x *= AUTO_JUMP_BREAK_SCALAR;
					m_velocity.z *= AUTO_JUMP_BREAK_SCALAR;

					return;
				}
			}
		}
	}	
}