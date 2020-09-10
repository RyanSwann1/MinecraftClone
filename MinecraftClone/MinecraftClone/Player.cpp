#include "Player.h"
#include <SFML/Graphics.hpp>
#include "ChunkManager.h"
#include <cmath>
#include <iostream>
#include "BoundingBox.h"
#include "PickUp.h"
#include "CollisionHandler.h"
#include "Gui.h"
#include "DestroyBlockVisual.h"
#include "SelectedVoxelVisual.h"
#include "GameMessenger.h"
#include "GameMessages.h"
#include <memory>

namespace 
{
	constexpr float MS_BLOCK_DESTROY = 0.5f;

	constexpr float WATER_MOVEMENT_SPEED = 0.2f;
	constexpr float WATER_GRAVITY = 0.02f;
	constexpr float WATER_JUMP_SPEED = 0.1f;

	constexpr float WALKING_MOVEMENT_SPEED = 0.55f;
	constexpr float FLYING_MOVEMENT_SPEED = 5.0f;
	constexpr glm::vec3 MAX_VELOCITY = { 50.f, 50.0f, 50.0 };
	constexpr float JUMP_SPEED = 12.0f;

	constexpr float RESISTENCE = 0.9f;
	constexpr float GRAVITY_AMOUNT = 1.0f;
	constexpr float HEAD_HEIGHT = 2.25f;
	constexpr int MS_BETWEEN_ATTEMPT_SPAWN = 250;
	
	constexpr float JUMP_BREAK = 1.0f;
	constexpr float MS_BETWEEN_JUMP = .25f;

	constexpr float DESTROY_BLOCK_MAX_DISTANCE = 5.0f;
	constexpr float DESTROY_BLOCK_INCREMENT = 0.5f;
	constexpr float PLACE_BLOCK_RANGE = 5.0f;
	constexpr float PLACE_BLOCK_INCREMENT = 0.1f;

	constexpr float COLLISION_OFFSET = 0.35f;
	
	constexpr float MS_BETWEEN_PLACE_CUBE = 0.25f;

	constexpr glm::vec3 DISCARD_ITEM_SPEED = { 10.0f, 5.7f, 10.0f };

	constexpr float SLOW_DESTROY_TIME = 4.0f;
	const CubeTypeComparison SLOW_DESTROYABLE_CUBE_TYPES =
	{
		{eCubeType::Stone,
		eCubeType::Log,
		eCubeType::LogTop}
	};

	constexpr float NORMAL_DESTROY_TIME = 0.75f;
	const CubeTypeComparison NORMAL_DESTROYABLE_CUBE_TYPES =
	{
		{eCubeType::Grass, 
		eCubeType::Dirt}
	};

	constexpr float FAST_DESTROY_TIME = 0.35f;
	const CubeTypeComparison FAST_DESTROYABLE_CUBE_TYPES =
	{
		{eCubeType::Sand,
		eCubeType::Leaves,
		eCubeType::Cactus,
		eCubeType::CactusTop}
	};

	const CubeTypeComparison INSTANT_DESTROYABLE_CUBE_TYPES =
	{
		{eCubeType::TallGrass,
		eCubeType::Shrub}
	};

	const CubeTypeComparison NON_DESTROYABLE_CUBE_TYPES =
	{
		{eCubeType::Water}
	};

	const CubeTypeComparison NON_COLLECTABLE_CUBE_TYPES =
	{
		{ eCubeType::Shrub,
		eCubeType::TallGrass,
		eCubeType::Leaves}
	};

	const CubeTypeComparison NON_SELECTABLE_CUBE_TYPES =
	{
		{eCubeType::Water}
	};
}

//Camera
Camera::Camera()
	: FOV(50.0f),
	sensitivity(4.0f),
	nearPlaneDistance(0.1f),
	farPlaneDistance(1750.f),
	front(),
	right(),
	up(),
	rotation(0.f, 5.f)
{
	front = { 0.0f, -1.0f, 0.0f };
	right = glm::normalize(glm::cross({ 0.0f, 1.0f, 0.0f }, front));
	up = { 0.0f, 1.0f, 0.0f };
}

void Camera::move(const sf::Window& window, float deltaTime)
{
	rotation.x += (static_cast<int>(window.getSize().y / 2) - sf::Mouse::getPosition(window).y) * sensitivity * deltaTime;
	rotation.y += (sf::Mouse::getPosition(window).x - static_cast<int>(window.getSize().x / 2)) * sensitivity * deltaTime;

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
	m_inventory(),
	m_cubeToDestroyPosition(),
	m_placeCubeTimer(MS_BETWEEN_PLACE_CUBE, true),
	m_destroyCubeTimer()
{
	m_jumpTimer.restart();

	GameMessenger::getInstance().subscribe<GameMessages::AddToInventory>(
		[this](const GameMessages::AddToInventory& gameMessage) { return onAddToInventory(gameMessage); }, this);
}

Player::~Player()
{
	GameMessenger::getInstance().unsubscribe<GameMessages::AddToInventory>(this);
}

const Timer& Player::getDestroyCubeTimer() const
{
	return m_destroyCubeTimer;
}

bool Player::isUnderWater(const ChunkManager& chunkManager, std::mutex& chunkInteractionMutex) const
{
	if (m_currentState != ePlayerState::InWater)
	{
		return false;
	}

	eCubeType cubeAtHeadPosition;
	std::lock_guard<std::mutex> playerLock(chunkInteractionMutex);
	if (chunkManager.isCubeAtPosition( { std::floor(m_position.x), std::floor(m_position.y) + 0.35f, std::floor(m_position.z) }, cubeAtHeadPosition))
	{
		assert(cubeAtHeadPosition != eCubeType::Air);
		
		return cubeAtHeadPosition == eCubeType::Water;
	}

	return false;
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

void Player::resetDestroyCubeTimer()
{
	m_destroyCubeTimer.resetElaspedTime();
	m_destroyCubeTimer.setActive(false);
}

void Player::placeBlock(ChunkManager& chunkManager)
{
	if (m_inventory.isSelectedItemEmpty())
	{
		return;
	}
	
	bool blockPlaced = false;
	for (int i = 0; i <= std::ceil(PLACE_BLOCK_RANGE / PLACE_BLOCK_INCREMENT); ++i)
	{
		glm::vec3 rayPosition = m_camera.front * (static_cast<float>(i) * PLACE_BLOCK_INCREMENT) + m_position;
		if (chunkManager.isCubeAtPosition({ std::floor(rayPosition.x), std::floor(rayPosition.y), std::floor(rayPosition.z) }))
		{
			for (int j = i; j >= 0; --j)
			{
				rayPosition = m_camera.front * (static_cast<float>(j) * PLACE_BLOCK_INCREMENT) + m_position;
				if (!chunkManager.isCubeAtPosition({ std::floor(rayPosition.x), std::floor(rayPosition.y), std::floor(rayPosition.z) }))
				{
					bool availablePostion = true;
					for (int y = -static_cast<int>(HEAD_HEIGHT); y <= 0; y++)
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
							m_inventory.reduceSelectedItem();
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
		for (int i = std::ceil(PLACE_BLOCK_RANGE / PLACE_BLOCK_INCREMENT); i >= 0; --i)
		{
			glm::vec3 rayPosition = m_camera.front * (static_cast<float>(i) * PLACE_BLOCK_INCREMENT) + m_position;
			eCubeType cubeTypeToPlace = m_inventory.getSelectedItemType();
			if (chunkManager.placeCubeAtPosition({ std::floor(rayPosition.x), std::floor(rayPosition.y), std::floor(rayPosition.z) }, cubeTypeToPlace))
			{
				m_inventory.reduceSelectedItem();
				break;
			}
		}
	}
}

void Player::destroyFacingBlock(ChunkManager& chunkManager)
{
	eCubeType cubeTypeToDestroy;
	if (m_destroyCubeTimer.isExpired() &&
		chunkManager.destroyCubeAtPosition(m_cubeToDestroyPosition, cubeTypeToDestroy))
	{
		assert(cubeTypeToDestroy != eCubeType::Air &&
			!NON_DESTROYABLE_CUBE_TYPES.isMatch(cubeTypeToDestroy));

		m_destroyCubeTimer.resetElaspedTime();
		m_destroyCubeTimer.setActive(false);

		if (!NON_COLLECTABLE_CUBE_TYPES.isMatch(cubeTypeToDestroy))
		{
			GameMessenger::getInstance().broadcast<GameMessages::SpawnPickUp>(
				{ cubeTypeToDestroy, m_cubeToDestroyPosition });
		}

		GameMessenger::getInstance().broadcast<GameMessages::DestroyCubeReset>({});
	}
	
	for (int i = 0; i <= std::ceil(DESTROY_BLOCK_MAX_DISTANCE / DESTROY_BLOCK_INCREMENT); ++i)
	{
		glm::vec3 rayPosition = m_camera.front * (static_cast<float>(i) * DESTROY_BLOCK_INCREMENT) + m_position;
		if (chunkManager.isCubeAtPosition({ std::floor(rayPosition.x), std::floor(rayPosition.y), std::floor(rayPosition.z) }, cubeTypeToDestroy) &&
			!NON_DESTROYABLE_CUBE_TYPES.isMatch(cubeTypeToDestroy))
		{
			if (!INSTANT_DESTROYABLE_CUBE_TYPES.isMatch(cubeTypeToDestroy))
			{
				if (!m_destroyCubeTimer.isActive() || glm::ivec3(std::floor(rayPosition.x), std::floor(rayPosition.y), std::floor(rayPosition.z)) !=
						m_cubeToDestroyPosition)
				{
					float destroyTime = 0.0f;
					if (SLOW_DESTROYABLE_CUBE_TYPES.isMatch(cubeTypeToDestroy))
					{
						destroyTime = SLOW_DESTROY_TIME;
					}
					else if (NORMAL_DESTROYABLE_CUBE_TYPES.isMatch(cubeTypeToDestroy))
					{
						destroyTime = NORMAL_DESTROY_TIME;
					}
					else if (FAST_DESTROYABLE_CUBE_TYPES.isMatch(cubeTypeToDestroy))
					{
						destroyTime = FAST_DESTROY_TIME;
					}
					assert(destroyTime != 0.0f);

					m_destroyCubeTimer.resetElaspedTime();
					m_cubeToDestroyPosition = { std::floor(rayPosition.x), std::floor(rayPosition.y), std::floor(rayPosition.z) };
					m_destroyCubeTimer.setNewExpirationTime(destroyTime);
					m_destroyCubeTimer.setActive(true);
					
					GameMessenger::getInstance().broadcast<GameMessages::DestroyCubeSetPosition>(
						{ m_cubeToDestroyPosition, m_destroyCubeTimer.getExpirationTime() });
				}
			}
			else
			{
				chunkManager.destroyCubeAtPosition(
					{ std::floor(rayPosition.x), std::floor(rayPosition.y), std::floor(rayPosition.z) }, cubeTypeToDestroy);
			}

			break;
		}
	}
}

void Player::spawn(const ChunkManager& chunkManager, std::mutex& chunkInteractionMutex)
{
	bool spawned = false;
	while (!spawned)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(MS_BETWEEN_ATTEMPT_SPAWN));

		std::lock_guard<std::mutex> playerLock(chunkInteractionMutex);
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
	if (!m_autoJump || m_velocity.y != 0 || Globals::getSqrMagnitude(m_position + m_velocity, m_position) <= 0.5f)
	{
		return;
	}

	eCubeType cubeType = eCubeType::Air;
	glm::vec3 collisionPosition(
		m_position.x + glm::normalize(glm::vec2(m_velocity.x, m_velocity.z)).x,
		m_position.y,
		m_position.z + glm::normalize(glm::vec2(m_velocity.x, m_velocity.z)).y);

	bool autoJumpAllowed = false;
	if (CollisionHandler::isCollision({ collisionPosition.x, collisionPosition.y - HEAD_HEIGHT + (HEAD_HEIGHT / 2.0f), collisionPosition.z }, chunkManager))
	{
		autoJumpAllowed = true;
		for (int y = -static_cast<int>(HEAD_HEIGHT) + 1; y <= 1; y++)
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

void Player::handleSelectedCube(const ChunkManager& chunkManager)
{
	bool selectedCubeFound = false;
	eCubeType selectedCubeType;
	for (int i = 0; i <= std::ceil(DESTROY_BLOCK_MAX_DISTANCE / DESTROY_BLOCK_INCREMENT); ++i)
	{
		glm::vec3 rayPosition = m_camera.front * (static_cast<float>(i) * DESTROY_BLOCK_INCREMENT) + m_position;
		if (chunkManager.isCubeAtPosition({ std::floor(rayPosition.x), std::floor(rayPosition.y), std::floor(rayPosition.z) }, selectedCubeType) &&
			!NON_SELECTABLE_CUBE_TYPES.isMatch(selectedCubeType))
		{
			GameMessenger::getInstance().broadcast<GameMessages::SelectedCubeSetPosition>(
				{ { std::floor(rayPosition.x), std::floor(rayPosition.y), std::floor(rayPosition.z) } });
			selectedCubeFound = true;
			break;
		}
	}

	if (!selectedCubeFound)
	{
		GameMessenger::getInstance().broadcast<GameMessages::SelectedCubeSetActive>({ false });
	}
}

void Player::onAddToInventory(const GameMessages::AddToInventory& gameMessage)
{
	m_inventory.add(gameMessage.type);
}

void Player::handleInputEvents(const sf::Event& currentSFMLEvent,
	ChunkManager& chunkManager, std::mutex& chunkInteractionMutex, const sf::Window& window)
{
	if (currentSFMLEvent.type == sf::Event::KeyPressed)
	{	
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::T))
		{
			m_autoJump = !m_autoJump;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
		{
			discardItem();
		}

		m_inventory.handleInputEvents(currentSFMLEvent);
	}
	if (currentSFMLEvent.type == sf::Event::MouseMoved)
	{
		handleSelectedCube(chunkManager);
	}
}

void Player::update(float deltaTime, std::mutex& chunkInteractionMutex, ChunkManager& chunkManager, const sf::Window& window)
{
	m_camera.move(window, deltaTime);
	m_placeCubeTimer.update(deltaTime);
	m_destroyCubeTimer.update(deltaTime);

	std::unique_lock<std::mutex> playerLock(chunkInteractionMutex);
	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
	{
		destroyFacingBlock(chunkManager);
	}
	else if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right) && m_placeCubeTimer.isExpired())
	{
		placeBlock(chunkManager);
		m_placeCubeTimer.resetElaspedTime();
	}

	for (int y = -static_cast<int>(HEAD_HEIGHT); y <= 0; y++)
	{
		if (CollisionHandler::isCollision({ std::floor(m_position.x), std::floor(m_position.y + y), std::floor(m_position.z) },
			chunkManager, eCubeType::Water))
		{
			m_currentState = ePlayerState::InWater;
			break;
		}
	}

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
	case ePlayerState::InWater:
		CollisionHandler::applyDrag(m_velocity.x, m_velocity.z, RESISTENCE);
		break;
	default:
		assert(false);
	}
}

void Player::move(float deltaTime, const ChunkManager& chunkManager)
{
	float movementSpeed = 0.0f;
	float jumpSpeed = 0.0f;
	float gravity = 0.0f;

	switch (m_currentState)
	{
	case ePlayerState::Flying:
		movementSpeed = FLYING_MOVEMENT_SPEED;
		break;
	case ePlayerState::InAir:
	case ePlayerState::OnGround:
		movementSpeed = WALKING_MOVEMENT_SPEED;
		gravity = GRAVITY_AMOUNT;
		jumpSpeed = JUMP_SPEED;
		break;
	case ePlayerState::InWater:
		movementSpeed = WATER_MOVEMENT_SPEED;
		jumpSpeed = WATER_JUMP_SPEED;
		gravity = WATER_GRAVITY;
		break;
	}

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
			m_velocity.y -= gravity;
		}
		break;
	case ePlayerState::OnGround:
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && m_velocity.y == 0 &&
			m_jumpTimer.getElapsedTime().asSeconds() >= MS_BETWEEN_JUMP)
		{
			m_requestingJump = true;
			m_jumpTimer.restart();
			m_velocity.y += jumpSpeed;

			if (m_velocity.x != 0 && m_velocity.z != 0)
			{
				glm::vec3 collisionPosition(
					m_position.x + glm::normalize(glm::vec2(m_velocity.x, m_velocity.z)).x,
					m_position.y - HEAD_HEIGHT,
					m_position.z + glm::normalize(glm::vec2(m_velocity.x, m_velocity.z)).y);

				if (CollisionHandler::isCollision(collisionPosition, chunkManager))
				{
					m_velocity.x *= JUMP_BREAK;
					m_velocity.z *= JUMP_BREAK;
				}
			}
		}

		break;
	case ePlayerState::InWater:
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			m_requestingJump = true;
			m_velocity.y += jumpSpeed;
		}
		else
		{
			m_velocity.y -= gravity;
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
		if (m_velocity.y > 0.0f && CollisionHandler::isCollision({ m_position.x, m_position.y + COLLISION_OFFSET, m_position.z }, chunkManager))
		{
			m_velocity.y = 0.0f;
			m_position.y -= std::abs(m_position.y - HEAD_HEIGHT - (std::floor(m_position.y - HEAD_HEIGHT)));
		}
		else if(m_velocity.y < 0.0f && CollisionHandler::isCollision({ m_position.x, m_position.y - HEAD_HEIGHT, m_position.z }, chunkManager))
		{
			m_velocity.y = 0.0f;
			m_position.y += std::abs(m_position.y - HEAD_HEIGHT - (std::floor(m_position.y - HEAD_HEIGHT) + 1));
			m_position.y = std::floor(m_position.y);

			m_currentState = ePlayerState::OnGround;	
		}

		if (m_velocity.x != 0.0f)
		{
			for (int y = -static_cast<int>(HEAD_HEIGHT); y <= 0; y++)
			{
				if (CollisionHandler::handleXAxisCollision(m_velocity.x, COLLISION_OFFSET, chunkManager, 
					{ m_position.x, m_position.y + y, m_position.z }))
				{
					break;
				}
			}
		}

		if (m_velocity.z != 0.0f)
		{
			for (int y = -static_cast<int>(HEAD_HEIGHT); y <= 0; y++)
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

		if (m_velocity.x != 0.0f)
		{
			for (int y = -static_cast<int>(HEAD_HEIGHT); y <= 0; y++)
			{
				if(CollisionHandler::handleXAxisCollision(m_velocity.x, COLLISION_OFFSET, chunkManager, 
					{ m_position.x, m_position.y + y, m_position.z }))
				{
					break;
				}
			}
		}

		if (m_velocity.z != 0.0f)
		{
			for (int y = -static_cast<int>(HEAD_HEIGHT); y <= 0; y++)
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
	case ePlayerState::InWater:
		if (m_velocity.y > 0.0f && CollisionHandler::isCollision({ m_position.x, m_position.y + COLLISION_OFFSET, m_position.z }, chunkManager))
		{
			m_velocity.y = 0.0f;
			m_position.y -= std::abs(m_position.y - HEAD_HEIGHT - (std::floor(m_position.y - HEAD_HEIGHT)));
		}
		else if (m_velocity.y < 0 && (CollisionHandler::isCollision({ m_position.x, m_position.y - HEAD_HEIGHT, m_position.z }, chunkManager)))
		{
			m_velocity.y = 0.0f;
			m_position.y += std::abs(m_position.y - HEAD_HEIGHT - (std::floor(m_position.y - HEAD_HEIGHT) + 1));
			//m_position.y = std::floor(m_position.y);
		}

		if (m_velocity.x != 0.0f)
		{
			for (int y = -static_cast<int>(HEAD_HEIGHT); y <= 0; y++)
			{
				if (CollisionHandler::handleXAxisCollision(m_velocity.x, COLLISION_OFFSET, chunkManager,
					{ m_position.x, m_position.y + y, m_position.z }))
				{
					break;
				}
			}
		}

		if (m_velocity.z != 0.0f)
		{
			for (int y = -static_cast<int>(HEAD_HEIGHT); y <= 0; y++)
			{
				if (CollisionHandler::handleZAxisCollision(m_velocity.z, COLLISION_OFFSET, chunkManager,
					{ m_position.x, m_position.y + y, m_position.z }))
				{
					break;
				}
			}
		}

		if (!CollisionHandler::isCollision({ m_position.x, m_position.y - (HEAD_HEIGHT + 1.5f), m_position.z }, chunkManager))
		{
			m_currentState = ePlayerState::InAir;
		}

		break;
	default:
		assert(false);
	}
}

void Player::discardItem()
{
	if (m_inventory.isSelectedItemEmpty() || m_currentState == ePlayerState::Flying)
	{
		return;
	}

	eCubeType pickUpType = m_inventory.getSelectedItemType();
	m_inventory.reduceSelectedItem();

	glm::vec3 spawnPosition = m_position;
	spawnPosition.x -= Globals::PICKUP_CUBE_FACE_SIZE / 2.0f;
	spawnPosition.z -= Globals::PICKUP_CUBE_FACE_SIZE / 2.0f;
	
	GameMessenger::getInstance().broadcast<GameMessages::PlayerDisgardPickup>(
		{ pickUpType, spawnPosition, glm::normalize(m_camera.front) * DISCARD_ITEM_SPEED });
}