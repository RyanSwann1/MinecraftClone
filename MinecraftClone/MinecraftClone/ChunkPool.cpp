#include "ChunkPool.h"
#include "Utilities.h"

ChunkPool::ChunkPool()
	: m_chunks()
{
	m_chunks.resize(static_cast<size_t>(1250));
}

Chunk* ChunkPool::getChunk(glm::ivec3 startingPosition)
{
	for (auto& chunk : m_chunks)
	{
		if (!chunk.isInUse())
		{
			chunk.reset(startingPosition);
			return &chunk;
		}
	}

	return nullptr;
}

void ChunkPool::increasePoolSize()
{
}
