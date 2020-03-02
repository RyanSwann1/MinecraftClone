#pragma once

#include "glm/glm.hpp"
#include "CubeID.h"
#include "Utilities.h"
#include <array>

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
	Chunk(glm::ivec3 startingPosition);
	
	bool isInUse() const;
	bool isPositionInBounds(glm::ivec3 position) const;
	glm::ivec3 getStartingPosition() const;
	CubeDetails getCubeDetails(glm::ivec3 position) const;
	CubeDetails getCubeDetailsAtPosition(glm::ivec3 position) const;

	void reset(glm::ivec3 startingPosition);
	void release();

private:
	bool m_inUse;
	glm::ivec3 m_startingPosition;
	glm::ivec3 m_endingPosition;
	std::array<std::array<std::array<CubeDetails, Utilities::CHUNK_DEPTH>, Utilities::CHUNK_HEIGHT>, Utilities::CHUNK_WIDTH> m_chunk;
	
	void regen(glm::ivec3 startingPosition);
};