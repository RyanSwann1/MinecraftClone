#pragma once

#include "NonCopyable.h"
#include "glm/glm.hpp"
#include "Globals.h"
#include "VertexArray.h"
#include "Rectangle.h"
#include "Timer.h"
#include <unordered_map>

//One VBO per pick up type
//Instancing
//Update position only and not whole mesh

class ShaderHandler;
class ChunkManager;
class Frustum;
class Player;
class Pickup : private NonCopyable
{
public:
	//On Player disgard
	Pickup(eCubeType cubeType, const glm::vec3& position, const glm::vec3& initialVelocity);
	//On destroyed Cube
	Pickup(eCubeType cubeType, const glm::ivec3& position);
	Pickup(Pickup&&) noexcept;
	Pickup& operator=(Pickup&&) noexcept;

	const glm::vec3& getPosition() const;
	eCubeType getCubeType() const;
	bool isInReachOfPlayer(const glm::vec3& playerMiddlePosition) const;

	void update(const Player& player, float deltaTime, const ChunkManager& chunkManager);
	void render(const Frustum& frustum, ShaderHandler& shaderHandler);

private:
	Timer m_collectionTimer;
	eCubeType m_cubeType;
	glm::vec3 m_position;
	glm::vec3 m_velocity;
	float m_yOffset;
	VertexArray m_vertexArray;
	bool m_onGround;
	float m_timeElasped;
};