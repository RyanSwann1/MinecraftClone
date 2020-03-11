#pragma once

#include "NonMovable.h"
#include "Chunk.h"
#include <vector>

class ChunkPool : private NonCopyable, private NonMovable
{
public:
	ChunkPool();

	Chunk& getChunk(const glm::ivec3& startingPosition);

private:
	std::vector<Chunk> m_chunks;
	Chunk* m_nextAvailable;
};