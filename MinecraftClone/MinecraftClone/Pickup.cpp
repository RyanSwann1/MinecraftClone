#include "Pickup.h"
#include "MeshGenerator.h"
#include "ChunkManager.h"
#include "Frustum.h"
#include "CollisionHandler.h"
#include "ShaderHandler.h"
#include "Player.h"

//PickUp
Pickup::Pickup(eCubeType cubeType, const glm::vec3& destroyedBlockPosition, const glm::vec3& initialVelocity)
	: m_AABB({ destroyedBlockPosition.x + 0.5f, destroyedBlockPosition.z + 0.5f }, 0.5f),
	m_cubeType(cubeType),
	m_position(destroyedBlockPosition),
	m_velocity(initialVelocity),
	m_movementSpeed(5.0f),
	m_vertexArray(),
	m_onGround(false),
	m_discardedByPlayer(true),
	m_timeElasped(0.0f)
{
	MeshGenerator::generatePickUpMesh(m_vertexArray.m_opaqueVertexBuffer, m_cubeType, m_position);
}

Pickup::Pickup(eCubeType cubeType, const glm::ivec3& destroyedBlockPosition)
	: m_AABB({ destroyedBlockPosition.x + 0.5f, destroyedBlockPosition.z + 0.5f }, 0.5f),
	m_cubeType(cubeType),
	m_position({ destroyedBlockPosition.x + 0.35f, destroyedBlockPosition.y + 0.35f, destroyedBlockPosition.z + 0.35f}),
	m_velocity(),
	m_movementSpeed(5.0f),
	m_vertexArray(),
	m_onGround(false),
	m_discardedByPlayer(false),
	m_timeElasped(0.0f)
{
	glm::vec3 n = glm::normalize(glm::vec3(Globals::getRandomNumber(0, 360), 90, Globals::getRandomNumber(0, 360)));
	m_velocity.x += n.x * 1.5f;
	m_velocity.y += n.y * 1.5f;
	m_velocity.z += n.z * 1.5f;

	MeshGenerator::generatePickUpMesh(m_vertexArray.m_opaqueVertexBuffer, m_cubeType, m_position);
}

Pickup::Pickup(Pickup&& orig) noexcept
	: m_AABB(orig.m_AABB),
	m_cubeType(orig.m_cubeType),
	m_position(orig.m_position),
	m_velocity(orig.m_velocity),
	m_movementSpeed(orig.m_movementSpeed),
	m_vertexArray(std::move(orig.m_vertexArray)),
	m_onGround(orig.m_onGround),
	m_discardedByPlayer(orig.m_discardedByPlayer),
	m_timeElasped(orig.m_timeElasped)
{}

Pickup& Pickup::operator=(Pickup&& orig) noexcept
{
	m_AABB = orig.m_AABB;
	m_cubeType = orig.m_cubeType;
	m_position = orig.m_position;
	m_velocity = orig.m_velocity;
	m_movementSpeed = orig.m_movementSpeed;
	m_vertexArray = std::move(orig.m_vertexArray);
	m_onGround = orig.m_onGround;
	m_discardedByPlayer = orig.m_discardedByPlayer;
	m_timeElasped = orig.m_timeElasped;

	return *this;
}

eCubeType Pickup::getCubeType() const
{
	return m_cubeType;
}

bool Pickup::isInReachOfPlayer(const glm::vec3& playerMiddlePosition) const
{
	return glm::distance(m_position, playerMiddlePosition) <= 0.4f;
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
		m_discardedByPlayer = false;
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

	glm::vec3 playerMiddlePosition = player.getPosition();
	if (!m_discardedByPlayer && glm::distance(m_position, playerMiddlePosition) <= 2.5f && 
		player.getInventory().isItemAddable(m_cubeType))
	{
		m_velocity += glm::normalize(glm::vec3(playerMiddlePosition - m_position)) * 5.0f;
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