#include "Pickup.h"
#include "MeshGenerator.h"
#include "ChunkManager.h"
#include "Frustum.h"
#include "CollisionHandler.h"
#include "ShaderHandler.h"
#include "Player.h"

namespace
{
	constexpr float MOVEMENT_SPEED = 5.0f;
	constexpr float MAXIMUM_DISTANCE_FROM_PLAYER = 2.5f;
	constexpr float MINIMUM_DISTANCE_FROM_PLAYER = 1.0f;
	constexpr float DESTROYED_CUBE_MIN_TIME_COLLECTION = 0.5f;
	constexpr float PLAYER_DISGARD_MIN_TIME_COLLECTION = 1.5;
}

Pickup::Pickup(eCubeType cubeType, const glm::vec3& position, const glm::vec3& initialVelocity)
	: m_collectionTimer(PLAYER_DISGARD_MIN_TIME_COLLECTION),
	m_AABB({ position.x + 0.5f, position.z + 0.5f }, 0.5f),
	m_cubeType(cubeType),
	m_position(position),
	m_velocity(initialVelocity),
	m_vertexArray(),
	m_onGround(false),
	m_timeElasped(0.0f)
{
	MeshGenerator::generatePickUpMesh(m_vertexArray.m_opaqueVertexBuffer, m_cubeType, m_position);
}

Pickup::Pickup(eCubeType cubeType, const glm::ivec3& position)
	: m_collectionTimer(DESTROYED_CUBE_MIN_TIME_COLLECTION),
	m_AABB({ position.x + 0.5f, position.z + 0.5f }, 0.5f),
	m_cubeType(cubeType),
	m_position({ position.x + 0.35f, position.y + 0.35f, position.z + 0.35f}),
	m_velocity(),
	m_vertexArray(),
	m_onGround(false),
	m_timeElasped(0.0f)
{
	glm::vec3 n = glm::normalize(glm::vec3(Globals::getRandomNumber(0, 360), 90, Globals::getRandomNumber(0, 360)));
	m_velocity.x += n.x * 1.5f;
	m_velocity.y += n.y * 1.5f;
	m_velocity.z += n.z * 1.5f;

	MeshGenerator::generatePickUpMesh(m_vertexArray.m_opaqueVertexBuffer, m_cubeType, m_position);
}

Pickup::Pickup(Pickup&& orig) noexcept
	: m_collectionTimer(orig.m_collectionTimer),
	m_AABB(orig.m_AABB),
	m_cubeType(orig.m_cubeType),
	m_position(orig.m_position),
	m_velocity(orig.m_velocity),
	m_vertexArray(std::move(orig.m_vertexArray)),
	m_onGround(orig.m_onGround),
	m_timeElasped(orig.m_timeElasped)
{}

Pickup& Pickup::operator=(Pickup&& orig) noexcept
{
	m_collectionTimer = orig.m_collectionTimer;
	m_AABB = orig.m_AABB;
	m_cubeType = orig.m_cubeType;
	m_position = orig.m_position;
	m_velocity = orig.m_velocity;
	m_vertexArray = std::move(orig.m_vertexArray);
	m_onGround = orig.m_onGround;
	m_timeElasped = orig.m_timeElasped;

	return *this;
}

eCubeType Pickup::getCubeType() const
{
	return m_cubeType;
}

bool Pickup::isInReachOfPlayer(const glm::vec3& playerMiddlePosition) const
{
	return glm::distance(m_position, playerMiddlePosition) <= MINIMUM_DISTANCE_FROM_PLAYER && !m_collectionTimer.isActive();
}

const Rectangle& Pickup::getAABB() const
{
	return m_AABB;
}

void Pickup::update(const Player& player, float deltaTime, const ChunkManager& chunkManager)
{
	if (CollisionHandler::isCollision({ m_position.x, m_position.y - 0.10f, m_position.z }, chunkManager))
	{
		m_velocity.y += 0.1f;
		m_onGround = false;
	}
	else if(!CollisionHandler::isCollision({ m_position.x, m_position.y - 0.10f - 0.5f, m_position.z }, chunkManager))
	{
		m_velocity.y -= 0.1f;
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
		m_position.y += glm::sin(m_timeElasped * 2.5f) * 0.005f;
	}

	m_collectionTimer.update(deltaTime);
	if (!m_collectionTimer.isActive() || m_collectionTimer.isExpired())
	{
		glm::vec3 playerMiddlePosition = player.getMiddlePosition();
		if (glm::distance(m_position, playerMiddlePosition) <= MAXIMUM_DISTANCE_FROM_PLAYER &&
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

	if (m_vertexArray.m_opaqueVertexBuffer.displayable && frustum.isItemInFrustum(m_position))
	{
		m_vertexArray.bindOpaqueVAO();

		glm::mat4 model = glm::translate(glm::mat4(1.0f), m_position);
		shaderHandler.setUniformMat4f(eShaderType::Pickup, "uModel", model);
		glDrawElements(GL_TRIANGLES, m_vertexArray.m_opaqueVertexBuffer.indicies.size(), GL_UNSIGNED_INT, nullptr);
	}
}