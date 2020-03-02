#pragma once

#include "NonCopyable.h"
#include "Chunk.h"
#include <vector>

class ChunkPool : private NonCopyable
{
public:
	ChunkPool();

	Chunk& getChunk(const glm::ivec3& startingPosition);

private:
	std::vector<Chunk> m_chunks;
};