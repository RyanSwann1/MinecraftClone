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

bool CollisionHandler::isGroundCollision(const glm::vec3& position, const ChunkManager& chunkManager)
{
	eCubeType cubeType = eCubeType::Air;

	return chunkManager.isCubeAtPosition({ std::floor(position.x), std::floor(position.y), std::floor(position.z) }, cubeType) ||
		Globals::NON_COLLIDABLE_CUBE_TYPES.isMatch(cubeType);
}

bool CollisionHandler::isCollision(const glm::vec3& position, const ChunkManager& chunkManager)
{
	eCubeType cubeType = eCubeType::Air;
	return chunkManager.isCubeAtPosition({ std::floor(position.x), std::floor(position.y), std::floor(position.z) }, cubeType) &&
		!Globals::NON_COLLIDABLE_CUBE_TYPES.isMatch(cubeType);
}