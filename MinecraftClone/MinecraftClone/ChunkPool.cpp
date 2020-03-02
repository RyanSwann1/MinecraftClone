#include "ChunkPool.h"

ChunkPool::ChunkPool()
{
	m_chunks.resize(size_t(2500));
}

Chunk& ChunkPool::getChunk(const glm::ivec3& startingPosition)
{
	for (auto& chunk : m_chunks)
	{
		if (!chunk.isInUse())
		{
			chunk.reset(startingPosition);
			return chunk;
		}
	}
}