#include "CollisionHandler.h"
#include "ChunkManager.h"
#include "Globals.h"

bool CollisionHandler::handleXAxisCollision(float& velocity, float offset, const ChunkManager& chunkManager, const glm::vec3& position)
{
	eCubeType cubeType = eCubeType::Air;
	assert(velocity != 0);
	glm::vec3 collisionPosition = position;
	if (velocity > 0)
	{
		collisionPosition.x += offset;
	}
	else
	{
		collisionPosition.x -= offset;
	}

	if (chunkManager.isCubeAtPosition({ std::floor(collisionPosition.x),
		std::floor(collisionPosition.y),
		std::floor(collisionPosition.z) }, cubeType) &&
		!Globals::NON_COLLIDABLE_CUBE_TYPES.isMatch(cubeType))
	{
		velocity = 0.0f;
		return true;
		//Handle when getting stuck
	}

	return false;
}

bool CollisionHandler::handleZAxisCollision(float& velocity, float offset, const ChunkManager& chunkManager, const glm::vec3& position)
{
	eCubeType cubeType = eCubeType::Air;
	assert(velocity != 0);
	glm::vec3 collisionPosition = position;
	if (velocity > 0)
	{
		collisionPosition.z += offset;
	}
	else
	{
		collisionPosition.z -= offset;
	}

	if (chunkManager.isCubeAtPosition({ std::floor(collisionPosition.x),
		std::floor(collisionPosition.y),
		std::floor(collisionPosition.z) }, cubeType) &&
		!Globals::NON_COLLIDABLE_CUBE_TYPES.isMatch(cubeType))
	{
		velocity = 0.0f;
		return true;
		//Handle when getting stuck
	}

	return false;
}

bool CollisionHandler::isCollision(const glm::vec3& position, const ChunkManager& chunkManager)
{
	eCubeType cubeType = eCubeType::Air;

	return chunkManager.isCubeAtPosition({ std::floor(position.x), std::floor(position.y), std::floor(position.z) }, cubeType) &&
		!Globals::NON_COLLIDABLE_CUBE_TYPES.isMatch(cubeType);
}

void CollisionHandler::applyDrag(glm::vec3& velocity, float resistence)
{
	velocity *= resistence;
	if (glm::abs(velocity.x) <= 0.02f)
	{
		velocity.x = 0.0f;
	}
	if (glm::abs(velocity.y) <= 0.02f)
	{
		velocity.y = 0.0f;
	}
	if (glm::abs(velocity.z) <= 0.02f)
	{
		velocity.z = 0.0f;
	}
}

void CollisionHandler::applyDrag(float& velocityX, float& velocityZ, float resistence)
{
	velocityX *= resistence;
	velocityZ *= resistence;

	if (glm::abs(velocityX) <= 0.02f)
	{
		velocityX = 0.0f;
	}
	if (glm::abs(velocityZ) <= 0.02f)
	{
		velocityZ = 0.0f;
	}
}