#include "Player.h"
#include <SFML/Graphics.hpp>
#include "ChunkManager.h"
#include <cmath>
#include <iostream>
#include "BoundingBox.h"
#include "PickUp.h"
#include "CollisionHandler.h"
#include "Gui.h"
#include <memory>

namespace 
{
	constexpr float WALKING_MOVEMENT_SPEED = 0.55f;
	constexpr float FLYING_MOVEMENT_SPEED = 5.0f;
	constexpr float WATER_MOVEMENT_SPEED = 0.1f;
	constexpr glm::vec3 MAX_VELOCITY = { 50.f, 50.0f, 50.0 };
	constexpr float JUMP_SPEED = 12.0f;

	constexpr float RESISTENCE = 0.9f;
	constexpr float GRAVITY_AMOUNT = 1.0f;
	constexpr float HEAD_HEIGHT = 2.25f;
	constexpr int MS_BETWEEN_ATTEMPT_SPAWN = 250;
	
	constexpr float JUMP_BREAK = 1.0f;
	constexpr float MS_BETWEEN_JUMP = .25f;

	constexpr float DESTROY_BLOCK_RANGE = 5.0f;
	constexpr float DESTROY_BLOCK_INCREMENT = 0.5f;
	constexpr float PLACE_BLOCK_RANGE = 5.0f;
	constexpr float PLACE_BLOCK_INCREMENT = 0.1f;

	constexpr float COLLISION_OFFSET = 0.35f;
	constexpr int BODY_HEIGHT = 2;

	constexpr glm::vec3 DISCARD_ITEM_SPEED = { 10.0f, 5.7f, 10.0f };

	const CubeTypeComparison NON_COLLECTABLE_CUBE_TYPES =
	{
		{ eCubeType::Shrub,
		eCubeType::TallGrass,
		eCubeType::Leaves}
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
	m_autoJump(false),
	m_requestingJump(false),
	m_jumpTimer(),
	m_inventory()
{
	m_jumpTimer.restart();
}

const glm::vec3 Player::getMiddlePosition() const
{
	return { m_position.x, m_position.y - 1, m_position.z };
}

const glm::vec3& Player::getPosition() const
{
	return m_position;
}

const Camera& Player::getCamera() const
{
	return m_camera;
}

const Inventory& Player::getInventory() const
{
	return m_inventory;
}

void Player::addToInventory(eCubeType cubeTypeToAdd, Gui& gui)
{
	m_inventory.add(cubeTypeToAdd, gui);
}

void Player::placeBlock(ChunkManager& chunkManager, std::mutex& playerMutex, Gui& gui)
{
	if (m_inventory.isSelectedItemEmpty())
	{
		return;
	}

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
					for (float y = -BODY_HEIGHT; y <= 0; y++)
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
						eCubeType cubeTypeToPlace = m_inventory.getSelectedItemType();
						if (chunkManager.placeCubeAtPosition({ std::floor(rayPosition.x), std::floor(rayPosition.y), std::floor(rayPosition.z) }, cubeTypeToPlace))
						{
							m_inventory.reduceSelectedItem(gui);
						}
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
			eCubeType cubeTypeToPlace = m_inventory.getSelectedItemType();
			if (chunkManager.placeCubeAtPosition({ std::floor(rayPosition.x), std::floor(rayPosition.y), std::floor(rayPosition.z) }, cubeTypeToPlace))
			{
				m_inventory.reduceSelectedItem(gui);
				break;
			}
		}
	}
}

void Player::destroyFacingBlock(ChunkManager& chunkManager, std::mutex& playerMutex, std::vector<Pickup>& pickUps) const
{
	std::lock_guard<std::mutex> playerLock(playerMutex);
	for (float i = 0; i <= DESTROY_BLOCK_RANGE; i += DESTROY_BLOCK_INCREMENT)
	{
		glm::vec3 rayPosition = m_camera.front * i + m_position;
		eCubeType destroyedCubeType;
		if (chunkManager.destroyCubeAtPosition({ std::floor(rayPosition.x), std::floor(rayPosition.y), std::floor(rayPosition.z) }, destroyedCubeType))
		{
			assert(destroyedCubeType != eCubeType::Air);
			if (!NON_COLLECTABLE_CUBE_TYPES.isMatch(destroyedCubeType))
			{
				pickUps.emplace_back(destroyedCubeType,
					glm::ivec3(std::floor(rayPosition.x), std::floor(rayPosition.y), std::floor(rayPosition.z)));
			}

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
		if (chunkManager.isChunkAtPosition(Globals::PLAYER_STARTING_POSITION))
		{
			m_position = chunkManager.getHighestCubeAtPosition(Globals::PLAYER_STARTING_POSITION);
			m_position.y += HEAD_HEIGHT;
			m_velocity = glm::vec3();
			m_currentState = ePlayerState::InAir;
			spawned = true;
		}
	}
}

void Player::handleAutoJump(const ChunkManager& chunkManager)
{
	if (!m_autoJump || m_velocity.y != 0 || glm::distance(m_position + m_velocity, m_position) < 0.5f)
	{
		return;
	}

	eCubeType cubeType = eCubeType::Air;
	glm::vec3 collisionPosition(
		m_position.x + glm::normalize(glm::vec2(m_velocity.x, m_velocity.z)).x,
		m_position.y,
		m_position.z + glm::normalize(glm::vec2(m_velocity.x, m_velocity.z)).y);

	bool autoJumpAllowed = false;
	if (CollisionHandler::isCollision({ collisionPosition.x, collisionPosition.y - BODY_HEIGHT, collisionPosition.z }, chunkManager))
	{
		autoJumpAllowed = true;
		for (int y = -BODY_HEIGHT + 1; y <= 1; y++)
		{
			if (CollisionHandler::isCollision({ collisionPosition.x, collisionPosition.y + y, collisionPosition.z }, chunkManager))
			{
				autoJumpAllowed = false;
				break;
			}
		}
	}

	if (autoJumpAllowed)
	{
		m_velocity.y += JUMP_SPEED;
		m_velocity.x *= JUMP_BREAK;
		m_velocity.z *= JUMP_BREAK;
	}
}

void Player::handleInputEvents(std::vector<Pickup>& pickUps, const sf::Event& currentSFMLEvent,
	ChunkManager& chunkManager, std::mutex& playerMutex, sf::Window& window, Gui& gui)
{
	if (currentSFMLEvent.type == sf::Event::KeyPressed)
	{	
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::T))
		{
			m_autoJump = !m_autoJump;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
		{
			discardItem(pickUps, gui);
		}

		m_inventory.handleInputEvents(currentSFMLEvent, gui);
	}
	if (currentSFMLEvent.type == sf::Event::MouseButtonPressed)
	{
		switch (currentSFMLEvent.mouseButton.button)
		{
		case sf::Mouse::Button::Left:
			break;

		}
		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
		{
			destroyFacingBlock(chunkManager, playerMutex, pickUps);
		}
		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
		{
			placeBlock(chunkManager, playerMutex, gui);
		}
	}
	if (currentSFMLEvent.MouseMoved)
	{
		m_camera.move(window);
	}
	if (currentSFMLEvent.type == currentSFMLEvent.MouseWheelScrolled)
	{
		m_inventory.handleInputEvents(currentSFMLEvent, gui);
	}
}

void Player::update(float deltaTime, std::mutex& playerMutex, const ChunkManager& chunkManager)
{
	std::unique_lock<std::mutex> playerLock(playerMutex);
	move(deltaTime, chunkManager);
	handleCollisions(chunkManager);
	playerLock.unlock();

	m_position += m_velocity * deltaTime;
	
	switch (m_currentState)
	{
	case ePlayerState::Flying:
		CollisionHandler::applyDrag(m_velocity, RESISTENCE);
		break;
	case ePlayerState::InAir:
	case ePlayerState::OnGround:
		CollisionHandler::applyDrag(m_velocity.x, m_velocity.z, RESISTENCE);
		break;
	default:
		assert(false);
	}
}

void Player::move(float deltaTime, const ChunkManager& chunkManager)
{
	float movementSpeed = (m_currentState == ePlayerState::Flying ? FLYING_MOVEMENT_SPEED : WALKING_MOVEMENT_SPEED);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
	{
		m_velocity.x += glm::cos(glm::radians(m_camera.rotation.y)) * movementSpeed;
		m_velocity.z += glm::sin(glm::radians(m_camera.rotation.y)) * movementSpeed;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
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

	if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
	{
		m_requestingJump = false;
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
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && !m_requestingJump)
		{
			m_currentState = ePlayerState::Flying;
		}
		else
		{
			m_velocity.y -= GRAVITY_AMOUNT;
		}
		break;
	case ePlayerState::OnGround:
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && m_velocity.y == 0 &&
			m_jumpTimer.getElapsedTime().asSeconds() >= MS_BETWEEN_JUMP)
		{
			m_requestingJump = true;
			m_jumpTimer.restart();
			m_velocity.y += JUMP_SPEED;

			if (m_velocity.x != 0 && m_velocity.z != 0)
			{
				glm::vec3 collisionPosition(
					m_position.x + glm::normalize(glm::vec2(m_velocity.x, m_velocity.z)).x,
					m_position.y - BODY_HEIGHT,
					m_position.z + glm::normalize(glm::vec2(m_velocity.x, m_velocity.z)).y);

				if (CollisionHandler::isCollision(collisionPosition, chunkManager))
				{
					m_velocity.x *= JUMP_BREAK;
					m_velocity.z *= JUMP_BREAK;
				}
			}
		}

		break;
	default:
		assert(false);
	}
}

//https://sites.google.com/site/letsmakeavoxelengine/home/collision-detection
void Player::handleCollisions(const ChunkManager& chunkManager)
{
	eCubeType cubeType = eCubeType::Air;
	switch (m_currentState)
	{
	case ePlayerState::Flying:
	case ePlayerState::InAir:
		if (CollisionHandler::isCollision({ m_position.x, m_position.y + COLLISION_OFFSET, m_position.z }, chunkManager))
		{
			m_velocity.y = 0;
			m_position.y -= std::abs(m_position.y - HEAD_HEIGHT - (std::floor(m_position.y - HEAD_HEIGHT)));
		}
		else if((CollisionHandler::isCollision({ m_position.x, m_position.y - HEAD_HEIGHT, m_position.z }, chunkManager)))
		{
			m_velocity.y = 0;
			m_position.y += std::abs(m_position.y - HEAD_HEIGHT - (std::floor(m_position.y - HEAD_HEIGHT) + 1));
			m_position.y = std::floor(m_position.y);

			m_currentState = ePlayerState::OnGround;	
		}

		if (m_velocity.x != 0)
		{
			for (int y = -BODY_HEIGHT; y <= 0; y++)
			{
				if (CollisionHandler::handleXAxisCollision(m_velocity.x, COLLISION_OFFSET, chunkManager, 
					{ m_position.x, m_position.y + y, m_position.z }))
				{
					break;
				}
			}
		}

		if (m_velocity.z != 0)
		{
			for (int y = -BODY_HEIGHT; y <= 0; y++)
			{
				if (CollisionHandler::handleZAxisCollision(m_velocity.z, COLLISION_OFFSET, chunkManager,
					{ m_position.x, m_position.y + y, m_position.z }))
				{
					break;
				}
			}
		}

		break;
	case ePlayerState::OnGround:
		handleAutoJump(chunkManager);

		if (m_velocity.x != 0)
		{
			for (int y = -BODY_HEIGHT; y <= 0; y++)
			{
				if(CollisionHandler::handleXAxisCollision(m_velocity.x, COLLISION_OFFSET, chunkManager, 
					{ m_position.x, m_position.y + y, m_position.z }))
				{
					break;
				}
			}
		}

		if (m_velocity.z != 0)
		{
			for (int y = -BODY_HEIGHT; y <= 0; y++)
			{
				if (CollisionHandler::handleZAxisCollision(m_velocity.z, COLLISION_OFFSET, chunkManager, 
					{ m_position.x, m_position.y + y, m_position.z }))
				{
					break;
				}
			}
		}
		
		if (!CollisionHandler::isCollision({ m_position.x, m_position.y - HEAD_HEIGHT, m_position.z }, chunkManager))
		{
			m_currentState = (m_currentState == ePlayerState::Flying ? m_currentState : ePlayerState::InAir);
		}

		break;
	default:
		assert(false);
	}
}

void Player::discardItem(std::vector<Pickup>& pickUps, Gui& gui)
{
	if (m_inventory.isSelectedItemEmpty() || m_currentState == ePlayerState::Flying)
	{
		return;
	}

	eCubeType pickUpType = m_inventory.getSelectedItemType();
	m_inventory.reduceSelectedItem(gui);

	glm::vec3 spawnPosition = m_position;
	spawnPosition.x -= Globals::PICKUP_CUBE_FACE_SIZE / 2.0f;
	spawnPosition.z -= Globals::PICKUP_CUBE_FACE_SIZE / 2.0f;
	
	pickUps.emplace_back(pickUpType, spawnPosition, glm::normalize(m_camera.front) * DISCARD_ITEM_SPEED);
}