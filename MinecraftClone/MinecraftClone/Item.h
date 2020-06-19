#pragma once

#include "glm/glm.hpp"
#include "Globals.h"
#include "VertexArray.h"
#include "Rectangle.h"
#include <unordered_map>

class ChunkManager;
struct PickUp
{
	PickUp(eCubeType cubeType, const glm::ivec3& destroyedBlockPosition);

	void update(const glm::vec3& playerPosition, float deltaTime, const ChunkManager& chunkManager);

	Rectangle m_AABB;
	eCubeType m_cubeType;
	glm::vec3 m_position;
	glm::vec3 m_velocity;
	float m_movementSpeed;
	VertexArray m_vertexArray;
	bool m_delete;
	bool m_onGround;
	float m_timeElasped;
};