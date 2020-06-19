#pragma once

#include "glm/glm.hpp"
#include "Globals.h"
#include "VertexArray.h"
#include "Rectangle.h"
#include <unordered_map>

class ChunkManager;
class Frustum;
class PickUp
{
public:
	PickUp(eCubeType cubeType, const glm::ivec3& destroyedBlockPosition);

	bool isInReachOfPlayer(const glm::vec3& playerPosition) const;
	const Rectangle& getAABB() const;

	void update(const glm::vec3& playerPosition, float deltaTime, const ChunkManager& chunkManager);
	void render(const Frustum& frustum);

private:
	Rectangle m_AABB;
	eCubeType m_cubeType;
	glm::vec3 m_position;
	glm::vec3 m_velocity;
	float m_movementSpeed;
	VertexArray m_vertexArray;
	bool m_onGround;
	float m_timeElasped;
};

class Item
{
public:
	Item(eCubeType cubeType);

	bool add();

private:
	const int m_max;
	eCubeType m_cubeType;
	int m_amount;
};
