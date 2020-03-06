#include "ChunkPool.h"
#include <iostream>

ChunkPool::ChunkPool()
{
	m_chunks.resize(size_t((5000)));

	for (int i = 0; i < static_cast<int>(m_chunks.size()) - 1; ++i)
	{
		m_chunks[i].setNext(&m_chunks[i + 1]);
	}

	m_nextAvailable = &m_chunks.front();
	m_chunks.back().setNext(m_nextAvailable);
}

Chunk& ChunkPool::getChunk(const glm::ivec3& startingPosition)
{
	int iterationCount = 0;
	bool validChunkFound = false;

	while (!validChunkFound)
	{
		assert(m_nextAvailable);
		if (m_nextAvailable->isInUse())
		{
			assert(m_nextAvailable->getNext());
			m_nextAvailable = m_nextAvailable->getNext();
		}
		else
		{
			validChunkFound = true;
		}

		assert(++iterationCount && iterationCount <= m_chunks.size());
	}
	
	m_nextAvailable->reuse(startingPosition);
	return *m_nextAvailable;
}