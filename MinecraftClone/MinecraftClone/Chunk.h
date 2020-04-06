#pragma once

#include "glm/glm.hpp"
#include "CubeID.h"
#include "Utilities.h"
#include "Rectangle.h"
#include "NonCopyable.h"
#include <array>

//Trees
//https://www.reddit.com/r/proceduralgeneration/comments/72waky/how_does_layered_generation_work_when_generating/
//https://www.reddit.com/r/proceduralgeneration/comments/2t2dyy/trees_in_minecraft_like_worlds_help/

//Saving/Storing
//https://www.reddit.com/r/proceduralgeneration/comments/3gwbux/question_how_does_the_world_remember_changes/

//position.y * (CHUNK_AREA) + position.z * CHUNK_SIZE + position.x;
class Chunk : private NonCopyable
{
public:
	Chunk();
	Chunk(const glm::ivec3& startingPosition);
	Chunk(Chunk&&) noexcept;
	Chunk& operator=(Chunk&&) noexcept;

	const Rectangle& getAABB() const;
	bool isInUse() const;
	bool isPositionInBounds(const glm::ivec3& position) const;
	const glm::ivec3& getStartingPosition() const;
	const glm::ivec3& getEndingPosition() const;
	char getCubeDetailsWithoutBoundsCheck(const glm::ivec3& position) const;
	
	void reuse(const glm::ivec3& startingPosition);
	void release();

private:
	bool m_inUse;
	glm::ivec3 m_startingPosition;
	glm::ivec3 m_endingPosition;
	std::array<char, Utilities::CHUNK_VOLUME> m_chunk;
	Rectangle m_AABB;

	bool isPositionInLocalBounds(const glm::ivec3& position) const;
	char getCubeAtLocalPosition(const glm::ivec3& position) const;
	int getElevationValue(int x, int y) const;
	float getMoistureValue(int x, int y) const;
	
	void changeCubeAtLocalPosition(const glm::ivec3& position, eCubeType cubeType);
	void regen(const glm::ivec3& startingPosition);
	void spawnWater();
	void spawnTrees();
	void spawnCactus();
	void spawnPlant(int maxQuantity, eCubeType baseCubeType, eCubeType plantCubeType);
	void spawnLeaves(const glm::ivec3& startingPosition);
	void spawnTreeStump(const glm::ivec3& startingPosition);
};