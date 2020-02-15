#pragma once

#include "glm/glm.hpp"
#include "CubeID.h"
#include <array>

struct CubeDetails
{
	CubeDetails()
		: type(eCubeType::Invalid),
		position()
	{}

	CubeDetails(eCubeType type, glm::vec3 position)
		: type(type),
		position(position)
	{}

	eCubeType type;
	glm::vec3 position;
};

//position.y * (CHUNK_AREA) + position.z * CHUNK_SIZE + position.x;
class Chunk
{
public:
	Chunk(glm::vec3 startingPosition);

	bool isPositionInBounds(glm::vec3 position) const;
	glm::vec3 getStartingPosition() const;
	const std::array<std::array<std::array<CubeDetails, 16>, 16>, 16> & getChunk() const;
	CubeDetails getCubeDetails(glm::vec3 position) const;
	
	void removeCubeAtPosition(glm::vec3 position);

private:
	glm::vec3 m_startingPosition;
	glm::vec3 m_endingPosition;

	std::array<std::array<std::array<CubeDetails, 16>, 16>, 16> m_chunk;
};