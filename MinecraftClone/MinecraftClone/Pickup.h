#pragma once

#include "NonCopyable.h"
#include "glm/glm.hpp"
#include "Globals.h"
#include "VertexArray.h"
#include "Rectangle.h"
#include <unordered_map>

//One VBO per pick up type
//Instancing
//Update position only and not whole mesh

class ShaderHandler;
class ChunkManager;
class Frustum;
class Pickup : private NonCopyable
{
public:
	Pickup(eCubeType cubeType, const glm::vec3& destroyedBlockPosition, const glm::vec3& initialVelocity);
	Pickup(eCubeType cubeType, const glm::ivec3& destroyedBlockPosition);
	Pickup(Pickup&&) noexcept;
	Pickup& operator=(Pickup&&) noexcept;

	eCubeType getCubeType() const;
	bool isInReachOfPlayer(const glm::vec3& playerPosition) const;
	const Rectangle& getAABB() const;

	void update(const glm::vec3& playerPosition, float deltaTime, const ChunkManager& chunkManager);
	void render(const Frustum& frustum, ShaderHandler& shaderHandler);

private:
	Rectangle m_AABB;
	eCubeType m_cubeType;
	glm::vec3 m_position;
	glm::vec3 m_velocity;
	float m_movementSpeed;
	VertexArray m_vertexArray;
	bool m_onGround;
	bool m_discardedByPlayer;
	float m_timeElasped;
};
