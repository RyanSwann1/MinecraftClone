#pragma once

#include "glm/glm.hpp"

class ChunkManager;
namespace CollisionHandler
{
	bool handleXAxisCollision(float& velocity, float offset, const ChunkManager& chunkManager, const glm::vec3& position);
	bool handleZAxisCollision(float& velocity, float offset, const ChunkManager& chunkManager, const glm::vec3& position);

	bool isGroundCollision(const glm::vec3& position, const ChunkManager& chunkManager);
	bool isCollision(const glm::vec3& position, const ChunkManager& chunkManager);
}