#include "ChunkPool.h"
#include <iostream>

ChunkPool::ChunkPool()
{
	m_chunks.resize(size_t(2500));
	
	m_nextAvailable = &m_chunks.front();

	for (int i = 0; i < m_chunks.size() - 1; ++i)
	{
		m_chunks[i].setNext(&m_chunks[i + 1]);
	}

	m_chunks.back().setNext(m_nextAvailable);
}

Chunk& ChunkPool::getChunk(const glm::ivec3& startingPosition)
{
	bool validChunkFound = false;
	while (!validChunkFound)
	{
		if (m_nextAvailable->isInUse())
		{
			m_nextAvailable = m_nextAvailable->getNext();
		}
		else
		{
			validChunkFound = true;
		}
	}
	
	m_nextAvailable->reuse(startingPosition);
	return *m_nextAvailable;
}

void ChunkPool::releaseChunk(Chunk& chunk)
{
	chunk.release();
}