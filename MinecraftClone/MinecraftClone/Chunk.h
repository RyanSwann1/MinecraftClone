#pragma once

#include "glm/glm.hpp"
#include "CubeID.h"
#include "Utilities.h"
#include <array>

//Trees
//https://www.reddit.com/r/proceduralgeneration/comments/72waky/how_does_layered_generation_work_when_generating/
//https://www.reddit.com/r/proceduralgeneration/comments/2t2dyy/trees_in_minecraft_like_worlds_help/

//Saving/Storing
//https://www.reddit.com/r/proceduralgeneration/comments/3gwbux/question_how_does_the_world_remember_changes/

struct CubeDetails
{
	CubeDetails()
		: type(static_cast<char>(eCubeType::Invalid))
	{}

	CubeDetails(eCubeType type)
		: type(static_cast<char>(type))
	{}

	char type;
};

//position.y * (CHUNK_AREA) + position.z * CHUNK_SIZE + position.x;
class Chunk
{
public:
	Chunk();
	Chunk(const glm::ivec3& startingPosition);
	
	bool isInUse() const;
	bool isPositionInBounds(const glm::ivec3& position) const;
	const glm::ivec3& getStartingPosition() const;
	const CubeDetails& getCubeDetailsWithoutBoundsCheck(const glm::ivec3& position) const;

	Chunk* getNext();
	void changeCubeAtPosition(const glm::vec3& position, eCubeType cubeType);
	void setNext(Chunk* chunk);
	void reuse(const glm::ivec3& startingPosition);
	void release();

private:
	bool m_inUse;
	glm::ivec3 m_startingPosition;
	glm::ivec3 m_endingPosition;
	std::array<std::array<std::array<CubeDetails, Utilities::CHUNK_DEPTH>, Utilities::CHUNK_HEIGHT>, Utilities::CHUNK_WIDTH> m_chunk;
	Chunk* m_next;

	bool isPositionInLocalBounds(const glm::ivec3& position) const;

	void regen(const glm::ivec3& startingPosition);
	void spawnWater();
	void spawnTrees();
	void spawnCactus();
	void spawnLeaves(const glm::ivec3& startingPosition, int distance);
};