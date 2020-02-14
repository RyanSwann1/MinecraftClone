#pragma once

#include "glm/glm.hpp"
#include <array>

class Chunk
{
public:
	Chunk(glm::vec3 startingPosition);

	glm::vec3 getStartingPosition() const;
	bool isPositionInChunk(glm::vec3 position) const;
	const std::array<std::array<std::array<glm::vec3, 16>, 16>, 16> & getChunk() const;

private:
	glm::vec3 m_startingPosition;
	std::array<std::array<std::array<glm::vec3, 16>, 16>, 16> m_chunk;
};