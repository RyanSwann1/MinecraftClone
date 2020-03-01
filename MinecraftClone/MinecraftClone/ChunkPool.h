#pragma once

#include <vector>
#include "Chunk.h"
#include "glm/glm.hpp"
#include <queue>

class ChunkPool
{
public:
	ChunkPool();

	Chunk* getChunk(glm::ivec3 startingPosition);

private:
	std::vector<Chunk> m_chunks;
};