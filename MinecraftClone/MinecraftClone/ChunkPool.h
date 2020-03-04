#pragma once

#include "NonCopyable.h"
#include "Chunk.h"
#include <vector>

class ChunkManager;
class ChunkPool : private NonCopyable
{
public:
	ChunkPool();

	Chunk& getChunk(const glm::ivec3& startingPosition, ChunkManager& chunkManager);
	void releaseChunk(Chunk& chunk);

private:
	std::vector<Chunk> m_chunks;
	Chunk* m_nextAvailable;
};