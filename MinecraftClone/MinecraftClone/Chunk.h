#pragma once

#include "glm/glm.hpp"
#include <array>

class Chunk
{
public:
	Chunk(glm::vec2 startingPosition);

	const std::array<std::array<std::array<glm::vec3, 16>, 16>, 16> & getChunk() const;

private:
	std::array<std::array<std::array<glm::vec3, 16>, 16>, 16> m_chunk;
};