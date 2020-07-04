#pragma once

#include "glm/glm.hpp"

class ChunkManager;
namespace CollisionHandler
{
	bool handleXAxisCollision(float& velocity, float offset, const ChunkManager& chunkManager, const glm::vec3& position);
	bool handleZAxisCollision(float& velocity, float offset, const ChunkManager& chunkManager, const glm::vec3& position);

	bool isCollision(const glm::vec3& position, const ChunkManager& chunkManager);
	void applyDrag(glm::vec3& velocity, float resistence);
	void applyDrag(float& velocityX, float& velocityZ, float resistence);
}