#include "ChunkPool.h"
#include <iostream>

//ChunkFromPool
ChunkFromPool::ChunkFromPool(ChunkPool& chunkPool, const glm::ivec3& startingPosition)
	: chunkPool(chunkPool),
	chunk(chunkPool.getChunk(startingPosition))
{}

ChunkFromPool::~ChunkFromPool()
{
	chunk.release();
}

//ChunkInPool
ChunkInPool::ChunkInPool()
	: chunk(),
	nextChunkInPool(nullptr)
{}

ChunkInPool::ChunkInPool(ChunkInPool&& orig) noexcept
	: chunk(std::move(orig.chunk)),
	nextChunkInPool(orig.nextChunkInPool)
{
	orig.nextChunkInPool = nullptr;
}

ChunkInPool& ChunkInPool::operator=(ChunkInPool&& orig) noexcept
{
	chunk = std::move(orig.chunk);
	nextChunkInPool = orig.nextChunkInPool;

	orig.nextChunkInPool = nullptr;

	return *this;
}

//ChunkPool
ChunkPool::ChunkPool()
{
	int x = Utilities::VISIBILITY_DISTANCE / Utilities::CHUNK_WIDTH;
	x += x += 2;
	int y = Utilities::VISIBILITY_DISTANCE / Utilities::CHUNK_DEPTH;
	y += y += 2;

	m_chunks.resize(size_t((x * y)));

	for (int i = 0; i < static_cast<int>(m_chunks.size()) - 1; ++i)
	{
		m_chunks[i].nextChunkInPool = &m_chunks[i + 1];
	}

	m_nextAvailable = &m_chunks.front();
	m_chunks.back().nextChunkInPool = m_nextAvailable;
}

Chunk& ChunkPool::getChunk(const glm::ivec3& startingPosition)
{
	int iterationCount = 0;
	bool validChunkFound = false;

	while (!validChunkFound)
	{
		assert(m_nextAvailable);
		if (m_nextAvailable->chunk.isInUse())
		{
			assert(m_nextAvailable);
			m_nextAvailable = m_nextAvailable->nextChunkInPool;
		}
		else
		{
			validChunkFound = true;
		}

		assert(++iterationCount && iterationCount <= m_chunks.size());
	}
	
	m_nextAvailable->chunk.reuse(startingPosition);
	return m_nextAvailable->chunk;
}