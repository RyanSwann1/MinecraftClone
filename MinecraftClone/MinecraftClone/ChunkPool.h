#pragma once

#include "NonCopyable.h"
#include "Chunk.h"
#include <vector>

class ChunkPool : private NonCopyable
{
public:
	ChunkPool();

	Chunk& getChunk(glm::ivec3 startingPosition);

private:
	std::vector<Chunk> m_chunks;
};