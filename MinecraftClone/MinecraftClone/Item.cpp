#include "Item.h"
#include "MeshGenerator.h"
#include "ChunkManager.h"
#include "Frustum.h"

//PickUp
PickUp::PickUp(eCubeType cubeType, const glm::vec3& destroyedBlockPosition, const glm::vec3& initialVelocity)
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

PickUp::PickUp(eCubeType cubeType, const glm::ivec3& destroyedBlockPosition)
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

eCubeType PickUp::getCubeType() const
{
	return m_cubeType;
}

bool PickUp::isInReachOfPlayer(const glm::vec3& playerPosition) const
{
	return glm::distance(m_position, { playerPosition.x, playerPosition.y - 1.0f, playerPosition.z }) <= 0.4f;
}

const Rectangle& PickUp::getAABB() const
{
	return m_AABB;
}

void PickUp::update(const glm::vec3& playerPosition, float deltaTime, const ChunkManager& chunkManager)
{
	eCubeType cubeType = eCubeType::Air;
	if (chunkManager.isCubeAtPosition({ std::floor(m_position.x),
		std::floor(m_position.y - 0.10f),
		std::floor(m_position.z) }, cubeType) &&
		!Globals::NON_COLLIDABLE_CUBE_TYPES.isMatch(cubeType))
	{
		m_velocity.y += 0.1f;
		m_onGround = false;
	}
	else if (!chunkManager.isCubeAtPosition({ std::floor(m_position.x),
		std::floor((m_position.y - 0.10f - 0.5f)),
		std::floor(m_position.z) }, cubeType) &&
		!Globals::NON_COLLIDABLE_CUBE_TYPES.isMatch(cubeType))
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

	for (float z = -0.25f; z <= 0.25f; z += 2)
	{ 
		if (chunkManager.isCubeAtPosition({ std::floor(m_position.x - 0.25f),
			std::floor(m_position.y),
			std::floor(m_position.z + z) }, cubeType) &&
			!Globals::NON_COLLIDABLE_CUBE_TYPES.isMatch(cubeType))
		{
			m_velocity.z = 0.0f;
			m_velocity.x = 0.0f;
			break;
		}
		else if (chunkManager.isCubeAtPosition({ std::floor(m_position.x + 0.25f),
			std::floor(m_position.y),
			std::floor(m_position.z + z) }, cubeType) &&
			!Globals::NON_COLLIDABLE_CUBE_TYPES.isMatch(cubeType))
		{
			m_velocity.z = 0.0f;
			m_velocity.x = 0.0f;
			break;
		}
	}

	for (float x = -0.25f; x <= 0.25f; x += 2)
	{
		if (chunkManager.isCubeAtPosition({ std::floor(m_position.x + x),
			std::floor(m_position.y),
			std::floor(m_position.z - 0.25f) }, cubeType) &&
			!Globals::NON_COLLIDABLE_CUBE_TYPES.isMatch(cubeType))
		{
			m_velocity.x = 0.0f;
			m_velocity.z = 0.0f;
			break;
		}
		else if (chunkManager.isCubeAtPosition({ std::floor(m_position.x + x),
			std::floor(m_position.y),
			std::floor(m_position.z + 0.25f) }, cubeType) &&
			!Globals::NON_COLLIDABLE_CUBE_TYPES.isMatch(cubeType))
		{
			m_velocity.x = 0.0f;
			m_velocity.z = 0.0f;
			break;
		}
	}

	
	if (m_onGround)
	{
		m_timeElasped += deltaTime;
		m_position.y += glm::sin(m_timeElasped * 2.5f) * 0.005f;
	}

	if (!m_discardedByPlayer && glm::distance(m_position, { playerPosition.x, playerPosition.y, playerPosition.z }) <= 2.5f)
	{
		m_velocity += glm::normalize(glm::vec3(glm::vec3(playerPosition.x, playerPosition.y - 1.0f, playerPosition.z) - m_position)) * 5.0f;
	}

	m_position += m_velocity * deltaTime;

	m_velocity.x *= 0.95f;
	m_velocity.z *= 0.95f;

	m_vertexArray.m_opaqueVertexBuffer.clear();
	MeshGenerator::generatePickUpMesh(m_vertexArray.m_opaqueVertexBuffer, m_cubeType, m_position);
}

void PickUp::render(const Frustum& frustum)
{
	if (m_vertexArray.m_opaqueVertexBuffer.bindToVAO)
	{
		m_vertexArray.attachOpaqueVBO();
	}

	if (m_vertexArray.m_opaqueVertexBuffer.displayable && frustum.isItemInFrustum(m_position))
	{
		m_vertexArray.bindOpaqueVAO();
		glDrawElements(GL_TRIANGLES, m_vertexArray.m_opaqueVertexBuffer.indicies.size(), GL_UNSIGNED_INT, nullptr);
	}
}

//Item
Item::Item(eCubeType cubeType)
	: m_cubeType(cubeType),
	m_currentAmount(1)
{}

bool Item::isEmpty() const
{
	return m_currentAmount == 0;
}

eCubeType Item::getCubeType() const
{
	assert(m_currentAmount > 0);
	return m_cubeType;
}

void Item::remove()
{
	assert(m_currentAmount > 0);
	--m_currentAmount;
}

void Item::add()
{
	++m_currentAmount;
}