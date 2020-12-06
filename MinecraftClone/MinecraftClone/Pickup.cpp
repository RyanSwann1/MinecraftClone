#include "Pickup.h"
#include "MeshGenerator.h"
#include "ChunkManager.h"
#include "Frustum.h"
#include "CollisionHandler.h"
#include "ShaderHandler.h"
#include "Player.h"
#include <iostream>

namespace
{
	constexpr float MOVEMENT_SPEED = 3.0f;
	constexpr float COLLISION_ADJUSTMENT_SPEED = 0.15f;
	constexpr float MAXIMUM_DISTANCE_FROM_PLAYER = 2.5f;
	constexpr float MINIMUM_DISTANCE_FROM_PLAYER = 1.0f;
	constexpr float DESTROYED_CUBE_MIN_TIME_COLLECTION = 0.5f;
	constexpr float PLAYER_DISGARD_MIN_TIME_COLLECTION = 1.5;
	constexpr glm::vec3 STARTING_POSITION_OFFSET = { 0.35f, 0.35f, 0.35f };
	constexpr glm::vec3 INITIAL_FORCE_AMPLIFIER = { 1.5f, 2.8f, 1.5f };
}

Pickup::Pickup(eCubeType cubeType, const glm::vec3& position, const glm::vec3& initialVelocity)
	: m_collectionTimer(PLAYER_DISGARD_MIN_TIME_COLLECTION),
	m_cubeType(cubeType),
	m_position(position),
	m_velocity(initialVelocity),
	m_yOffset(0.0f),
	m_vertexArray(),
	m_onGround(false),
	m_timeElasped(0.0f)
{
	MeshGenerator::generatePickUpMesh(m_vertexArray.m_opaqueVertexBuffer, m_cubeType);
}

Pickup::Pickup(eCubeType cubeType, const glm::vec3& position)
	: m_collectionTimer(DESTROYED_CUBE_MIN_TIME_COLLECTION),
	m_cubeType(cubeType),
	m_position(position + STARTING_POSITION_OFFSET),
	m_velocity(),
	m_yOffset(0.0f),
	m_vertexArray(),
	m_onGround(false),
	m_timeElasped(0.0f)
{
	glm::vec2 n = glm::normalize(glm::vec2(Globals::getRandomNumber(-1.0f, 1.0f), Globals::getRandomNumber(-1.0f, 1.0f)));
	m_velocity += glm::vec3(INITIAL_FORCE_AMPLIFIER.x * n.x, INITIAL_FORCE_AMPLIFIER.y, INITIAL_FORCE_AMPLIFIER.z * n.y);

	MeshGenerator::generatePickUpMesh(m_vertexArray.m_opaqueVertexBuffer, m_cubeType);
}

Pickup::Pickup(Pickup&& orig) noexcept
	: m_collectionTimer(orig.m_collectionTimer),
	m_cubeType(orig.m_cubeType),
	m_position(orig.m_position),
	m_velocity(orig.m_velocity),
	m_yOffset(orig.m_yOffset),
	m_vertexArray(std::move(orig.m_vertexArray)),
	m_onGround(orig.m_onGround),
	m_timeElasped(orig.m_timeElasped)
{}

Pickup& Pickup::operator=(Pickup&& orig) noexcept
{
	m_collectionTimer = orig.m_collectionTimer;
	m_cubeType = orig.m_cubeType;
	m_position = orig.m_position;
	m_velocity = orig.m_velocity;
	m_yOffset = orig.m_yOffset;
	m_vertexArray = std::move(orig.m_vertexArray);
	m_onGround = orig.m_onGround;
	m_timeElasped = orig.m_timeElasped;

	return *this;
}

const glm::vec3& Pickup::getPosition() const
{
	return m_position;
}

eCubeType Pickup::getCubeType() const
{
	return m_cubeType;
}

bool Pickup::isInReachOfPlayer(const glm::vec3& playerMiddlePosition) const
{
	return Globals::getSqrMagnitude(m_position, playerMiddlePosition) <= MINIMUM_DISTANCE_FROM_PLAYER * MINIMUM_DISTANCE_FROM_PLAYER &&
		!m_collectionTimer.isActive();
}

void Pickup::update(const Player& player, float deltaTime, const ChunkManager& chunkManager)
{
	if (CollisionHandler::isCollision({ m_position.x, m_position.y, m_position.z }, chunkManager))
	{
		m_velocity.y += COLLISION_ADJUSTMENT_SPEED;
		m_onGround = false;
	}
	else if(!CollisionHandler::isCollision({ m_position.x, m_position.y - Globals::PICKUP_CUBE_FACE_SIZE * 1.25f, m_position.z }, chunkManager))
	{
		m_velocity.y -= COLLISION_ADJUSTMENT_SPEED;
		m_onGround = false;
	}
	else
	{
		m_onGround = true;
		m_velocity.y = 0.0f;
	}
		
	if (m_velocity.x != 0)
	{
		CollisionHandler::handleXAxisCollision(m_velocity.x, 0.25f, chunkManager, m_position);
	}

	if (m_velocity.z != 0)
	{
		CollisionHandler::handleZAxisCollision(m_velocity.z, 0.25f, chunkManager, m_position);
	}

	if (m_onGround)
	{
		m_timeElasped += deltaTime;
		m_yOffset = glm::sin(m_timeElasped * 2.0f) * 0.1f; 
	}

	m_collectionTimer.update(deltaTime);
	if (m_collectionTimer.isExpired())
	{
		m_collectionTimer.setActive(false);
	}
	
	if (!m_collectionTimer.isActive())
	{
		glm::vec3 playerMiddlePosition = player.getMiddlePosition();
		if (Globals::getSqrMagnitude(m_position, playerMiddlePosition) <= MAXIMUM_DISTANCE_FROM_PLAYER * MAXIMUM_DISTANCE_FROM_PLAYER &&
			player.getInventory().isItemAddable(m_cubeType))
		{
			m_velocity += glm::normalize(glm::vec3(playerMiddlePosition - m_position)) * MOVEMENT_SPEED;
		}
	}

	m_position += m_velocity * deltaTime;

	CollisionHandler::applyDrag(m_velocity.x, m_velocity.z, 0.95f);
}

void Pickup::render(const Frustum& frustum, ShaderHandler& shaderHandler)
{
	if (m_vertexArray.m_opaqueVertexBuffer.bindToVAO)
	{
		m_vertexArray.attachOpaqueVBO();
	}

	if (m_vertexArray.m_opaqueVertexBuffer.displayable && frustum.isPositionInFrustum(m_position))
	{
		m_vertexArray.bindOpaqueVAO();

		glm::vec3 position = m_position + glm::vec3(0, m_yOffset, 0);
		glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
		shaderHandler.setUniformMat4f(eShaderType::Pickup, "uModel", model);

		glDrawElements(GL_TRIANGLES, m_vertexArray.m_opaqueVertexBuffer.indicies.size(), GL_UNSIGNED_INT, nullptr);
	}
}