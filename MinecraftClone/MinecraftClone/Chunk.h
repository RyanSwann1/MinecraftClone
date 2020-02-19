#pragma once

#include "glm/glm.hpp"
#include "CubeID.h"
#include "Utilities.h"
#include <array>

struct CubeDetails
{
	CubeDetails()
		: type(eCubeType::Invalid),
		position()
	{}

	CubeDetails(eCubeType type, glm::ivec3 position)
		: type(type),
		position(position)
	{}

	eCubeType type;
	glm::ivec3 position;
};

//position.y * (CHUNK_AREA) + position.z * CHUNK_SIZE + position.x;
class Chunk
{
public:
	Chunk(glm::ivec3 startingPosition);
	
	bool isPositionInBounds(glm::ivec2 position) const;
	bool isPositionInBounds(glm::vec3 position) const;
	bool isPositionInBounds(glm::ivec3 position) const;
	glm::ivec3 getEndingPosition() const;
	glm::ivec3 getStartingPosition() const;
	CubeDetails getCubeDetails(glm::ivec3 position) const;
	CubeDetails getCubeDetailsAtPosition(glm::ivec3 position) const;

	void removeCubeAtPosition(glm::ivec3 position);

private:
	glm::ivec3 m_startingPosition;
	glm::ivec3 m_endingPosition;

	std::array<std::array<std::array<CubeDetails, Utilities::CHUNK_DEPTH>, Utilities::CHUNK_HEIGHT>, Utilities::CHUNK_WIDTH> m_chunk;
};