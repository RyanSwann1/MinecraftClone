#include "Player.h"
#include <SFML/Graphics.hpp>
#include "ChunkManager.h"
#include <cmath>
#include <iostream>
#include "BoundingBox.h"

namespace 
{
	constexpr float WALKING_MOVEMENT_SPEED = 0.55f;
	constexpr float FLYING_MOVEMENT_SPEED = 5.0f;
	constexpr glm::vec3 MAX_VELOCITY = { 50.f, 50.0f, 50.0 };
	constexpr float JUMP_SPEED = 12.0f;

	constexpr float DRAG_AMOUNT = 0.9f;
	constexpr float GRAVITY_AMOUNT = 1.0f;
	constexpr float HEAD_HEIGHT = 2.25f;
	constexpr int MS_BETWEEN_ATTEMPT_SPAWN = 250;
	
	constexpr float AUTO_JUMP_HEIGHT = 2.0f;
	constexpr float JUMP_BREAK = 1.0f;

	constexpr float DESTROY_BLOCK_RANGE = 5.0f;
	constexpr float DESTROY_BLOCK_INCREMENT = 0.5f;
	constexpr float PLACE_BLOCK_RANGE = 5.0f;
	constexpr float PLACE_BLOCK_INCREMENT = 0.1f;

	constexpr float COLLISION_OFFSET = 0.35f;

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
	m_velocity(),
	m_autoJump(false)
{}

const glm::vec3& Player::getPosition() const
{
	return m_position;
}

const Camera& Player::getCamera() const
{
	return m_camera;
}

void Player::placeBlock(ChunkManager& chunkManager, std::mutex& playerMutex)
{
	std::lock_guard<std::mutex> playerLock(playerMutex);

	bool blockPlaced = false;
	for (float i = 0; i <= PLACE_BLOCK_RANGE; i += PLACE_BLOCK_INCREMENT)
	{
		glm::vec3 rayPosition = m_camera.front * i + m_position;
		if (chunkManager.isCubeAtPosition({ std::floor(rayPosition.x), std::floor(rayPosition.y), std::floor(rayPosition.z) }))
		{
			for (float j = i - PLACE_BLOCK_INCREMENT; j >= 0; j -= PLACE_BLOCK_INCREMENT)
			{
				rayPosition = m_camera.front * j + m_position;
				if (!chunkManager.isCubeAtPosition({ std::floor(rayPosition.x), std::floor(rayPosition.y), std::floor(rayPosition.z) }))
				{
					bool availablePostion = true;
					for (float y = -AUTO_JUMP_HEIGHT; y <= 0; y++)
					{
						if (glm::vec3(std::floor(m_position.x), std::floor(m_position.y - 2.0f), std::floor(m_position.z)) == 
						glm::vec3(std::floor(rayPosition.x), std::floor(rayPosition.y + y), std::floor(rayPosition.z) ))
						{
							availablePostion = false;
							break;
						}
					}

					if (availablePostion)
					{
						chunkManager.placeCubeAtPosition({ std::floor(rayPosition.x), std::floor(rayPosition.y), std::floor(rayPosition.z) });
					}
					
					blockPlaced = true;
					break;
				}
			}

			break;
		}
	}

	if (!blockPlaced)
	{
		for (float i = PLACE_BLOCK_RANGE; i >= 0; i -= PLACE_BLOCK_INCREMENT)
		{
			glm::vec3 rayPosition = m_camera.front * i + m_position;
			if (chunkManager.placeCubeAtPosition({ std::floor(rayPosition.x), std::floor(rayPosition.y), std::floor(rayPosition.z) }))
			{
				break;
			}
		}
	}
}

void Player::destroyFacingBlock(ChunkManager& chunkManager, std::mutex& playerMutex)
{
	std::lock_guard<std::mutex> playerLock(playerMutex);
	for (float i = 0; i <= DESTROY_BLOCK_RANGE; i += DESTROY_BLOCK_INCREMENT)
	{
		glm::vec3 rayPosition = m_camera.front * i + m_position;
		if (chunkManager.destroyCubeAtPosition({ std::floor(rayPosition.x), std::floor(rayPosition.y), std::floor(rayPosition.z) }))
		{
			break;
		}
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

void Player::toggleAutoJump()
{
	m_autoJump = !m_autoJump;
}

void Player::moveCamera(const sf::Window& window)
{
	m_camera.move(window);
}

void Player::update(float deltaTime, std::mutex& playerMutex, const ChunkManager& chunkManager)
{
	move(deltaTime, playerMutex, chunkManager);
	
	std::lock_guard<std::mutex> playerLock(playerMutex);
	handleCollisions(chunkManager, deltaTime);
	
	m_position += m_velocity * deltaTime;
	applyDrag();
}

void Player::move(float deltaTime, std::mutex& playerMutex, const ChunkManager& chunkManager)
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
			if (m_velocity.x != 0 && m_velocity.z != 0)
			{
				eCubeType cubeType;
				std::lock_guard<std::mutex> playerLock(playerMutex);
				glm::vec3 collisionPosition(
					m_position.x + glm::normalize(glm::vec2(m_velocity.x, m_velocity.z)).x,
					m_position.y,
					m_position.z + glm::normalize(glm::vec2(m_velocity.x, m_velocity.z)).y);

				if (chunkManager.isCubeAtPosition({ std::floor(collisionPosition.x),
					std::floor(collisionPosition.y - AUTO_JUMP_HEIGHT),
					std::floor(collisionPosition.z) }, cubeType) &&
					!NON_COLLIDABLE_CUBE_TYPES.isMatch(cubeType))
				{
					m_velocity.y += JUMP_SPEED;
					m_velocity.x *= JUMP_BREAK;
					m_velocity.z *= JUMP_BREAK;
				}
				else
				{
					m_velocity.y += JUMP_SPEED;
				}
			}
			else
			{
				m_velocity.y += JUMP_SPEED;
			}
		}
		break;
	default:
		assert(false);
	}
}

void Player::applyDrag()
{
	switch (m_currentState)
	{
	case ePlayerState::Flying:

		m_velocity *= DRAG_AMOUNT;
		if (glm::abs(m_velocity.x) <= 0.02f)
		{
			m_velocity.x = 0.0f;
		}
		if (glm::abs(m_velocity.y) <= 0.02f)
		{
			m_velocity.y = 0.0f;
		}
		if (glm::abs(m_velocity.z) <= 0.02f)
		{
			m_velocity.z = 0.0f;
		}
		break;
	case ePlayerState::InAir:
	case ePlayerState::OnGround:
		m_velocity.x *= DRAG_AMOUNT;
		m_velocity.z *= DRAG_AMOUNT;

		if (glm::abs(m_velocity.x) <= 0.02f)
		{
			m_velocity.x = 0.0f;
		}
		if (glm::abs(m_velocity.z) <= 0.02f)
		{
			m_velocity.z = 0.0f;
		}
		break;
	default:
		assert(false);
	}
}

//https://sites.google.com/site/letsmakeavoxelengine/home/collision-detection
void Player::handleCollisions(const ChunkManager& chunkManager, float deltaTime)
{
	eCubeType cubeType = eCubeType::Air;
	switch (m_currentState)
	{
	case ePlayerState::Flying:
	case ePlayerState::InAir:
		if (chunkManager.isCubeAtPosition({ std::floor(m_position.x), std::floor(m_position.y + COLLISION_OFFSET), std::floor(m_position.z) }, cubeType) &&
			!NON_COLLIDABLE_CUBE_TYPES.isMatch(cubeType))
		{
			m_velocity.y = 0;
			m_position.y -= std::abs(m_position.y - HEAD_HEIGHT - (std::floor(m_position.y - HEAD_HEIGHT)));
		}

		if (m_velocity.x != 0)
		{
			for (int y = -AUTO_JUMP_HEIGHT; y <= 0; y++)
			{
				glm::vec3 collisionPosition;
				if (m_velocity.x > 0)
				{
					collisionPosition = {
						m_position.x + COLLISION_OFFSET,
						m_position.y,
						m_position.z };
				}
				else
				{
					collisionPosition = {
						m_position.x - COLLISION_OFFSET,
						m_position.y,
						m_position.z };
				}

				if (chunkManager.isCubeAtPosition({ std::floor(collisionPosition.x),
					std::floor(collisionPosition.y + y),
					std::floor(collisionPosition.z) }, cubeType) &&
					!NON_COLLIDABLE_CUBE_TYPES.isMatch(cubeType))
				{
					m_velocity.x = 0.0f;
					//Handle when getting stuck
					break;
				}
			}

		}

		//Collide into faces - walking - Z
		if (m_velocity.z != 0)
		{
			for (int y = -AUTO_JUMP_HEIGHT; y <= 0; y++)
			{
				glm::vec3 collisionPosition;
				if (m_velocity.z > 0)
				{
					collisionPosition = {
						m_position.x,
						m_position.y,
						m_position.z + COLLISION_OFFSET };
				}
				else
				{
					collisionPosition = {
						m_position.x,
						m_position.y,
						m_position.z - COLLISION_OFFSET };
				}

				if (chunkManager.isCubeAtPosition({ std::floor(collisionPosition.x),
					std::floor(collisionPosition.y + y),
					std::floor(collisionPosition.z) }, cubeType) &&
					!NON_COLLIDABLE_CUBE_TYPES.isMatch(cubeType))
				{
					m_velocity.z = 0.0f;
					//Handle when getting stuck
					break;
				}
			}
		}

		if (chunkManager.isCubeAtPosition({ std::floor(m_position.x), 
			std::floor(m_position.y - HEAD_HEIGHT), 
			std::floor(m_position.z) }, cubeType) &&
			!NON_COLLIDABLE_CUBE_TYPES.isMatch(cubeType))
		{
			m_velocity.y = 0;
			m_position.y += std::abs(m_position.y - HEAD_HEIGHT - (std::floor(m_position.y - HEAD_HEIGHT) + 1));
			m_position.y = std::floor(m_position.y);

			m_currentState = ePlayerState::OnGround;
		}
		break;
	case ePlayerState::OnGround:
		//Auto Jump
		if(m_autoJump && m_velocity.y == 0 && glm::distance(m_position + m_velocity, m_position) >= 0.5f)
		{
			glm::vec3 collisionPosition(
				m_position.x + glm::normalize(glm::vec2(m_velocity.x, m_velocity.z)).x,
				m_position.y,
				m_position.z + glm::normalize(glm::vec2(m_velocity.x, m_velocity.z)).y);

			if (chunkManager.isCubeAtPosition({ std::floor(collisionPosition.x),
				std::floor(collisionPosition.y - AUTO_JUMP_HEIGHT),
				std::floor(collisionPosition.z) }, cubeType) &&
				!NON_COLLIDABLE_CUBE_TYPES.isMatch(cubeType))
			{
				bool autoJumpAllowed = true;
				for (int y = -AUTO_JUMP_HEIGHT + 1; y <= 1; y++)
				{
					if (chunkManager.isCubeAtPosition({ std::floor(collisionPosition.x),
						std::floor(collisionPosition.y + y),
						std::floor(collisionPosition.z) }, cubeType) &&
						!NON_COLLIDABLE_CUBE_TYPES.isMatch(cubeType))
					{
						autoJumpAllowed = false;
						break;
					}
				}

				if (autoJumpAllowed)
				{
					m_velocity.y += JUMP_SPEED;
					m_velocity.x *= JUMP_BREAK;
					m_velocity.z *= JUMP_BREAK;
				}
			}
		}


		//Collide into faces - walking - X
		if (m_velocity.x != 0)
		{
			for (int y = -AUTO_JUMP_HEIGHT; y <= 0; y++)
			{
				glm::vec3 collisionPosition;
				if (m_velocity.x > 0)
				{
					collisionPosition = {
						m_position.x + COLLISION_OFFSET, //Positive & negative check
						m_position.y,
						m_position.z};
				}
				else
				{
					collisionPosition = {
						m_position.x - COLLISION_OFFSET, //Positive & negative check
						m_position.y,
						m_position.z };
				}

				if (chunkManager.isCubeAtPosition({ std::floor(collisionPosition.x),
					std::floor(collisionPosition.y + y),
					std::floor(collisionPosition.z) }, cubeType) &&
					!NON_COLLIDABLE_CUBE_TYPES.isMatch(cubeType))
				{
					m_velocity.x = 0.0f;
					//Handle when getting stuck
					break;
				}
			}
		}

		//Collide into faces - walking - Z
		if (m_velocity.z != 0)
		{
			for (int y = -AUTO_JUMP_HEIGHT; y <= 0; y++)
			{
				glm::vec3 collisionPosition;
				if (m_velocity.z > 0)
				{
					collisionPosition = {
						m_position.x, //Positive & negative check
						m_position.y,
						m_position.z + COLLISION_OFFSET };
				}
				else
				{
					collisionPosition = {
						m_position.x, //Positive & negative check
						m_position.y,
						m_position.z - COLLISION_OFFSET };
				}

				if (chunkManager.isCubeAtPosition({ std::floor(collisionPosition.x),
					std::floor(collisionPosition.y + y),
					std::floor(collisionPosition.z) }, cubeType) &&
					!NON_COLLIDABLE_CUBE_TYPES.isMatch(cubeType))
				{
					m_velocity.z = 0.0f;
					//Handle when getting stuck
					break;
				}
			}
		}
		
		if (!chunkManager.isCubeAtPosition({ std::floor(m_position.x), std::floor(m_position.y - HEAD_HEIGHT), std::floor(m_position.z) }, cubeType) ||
			NON_COLLIDABLE_CUBE_TYPES.isMatch(cubeType))
		{
			m_currentState = (m_currentState == ePlayerState::Flying ? m_currentState : ePlayerState::InAir);
		}
		break;
	default:
		assert(false);
	}
}