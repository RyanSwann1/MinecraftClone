#include "ChunkPool.h"
#include <iostream>

//ChunkFromPool
ChunkFromPool::ChunkFromPool(ChunkPool& chunkPool, const glm::ivec3& startingPosition)
	: ObjectFromPool(chunkPool.getChunk(startingPosition), chunkPool)
{}

ChunkFromPool::~ChunkFromPool()
{
	object.release();
}

//ChunkPool
ChunkPool::ChunkPool()
{
	int x = Utilities::VISIBILITY_DISTANCE / Utilities::CHUNK_WIDTH;
	x += x += 2;
	int y = Utilities::VISIBILITY_DISTANCE / Utilities::CHUNK_DEPTH;
	y += y += 2;

	m_objectPool.resize(size_t((x * y)));

	for (int i = 0; i < static_cast<int>(m_objectPool.size()) - 1; ++i)
	{
		m_objectPool[i].nextAvailableObject = &m_objectPool[i + 1];
	}

	m_nextAvailableObject = &m_objectPool.front();
	m_objectPool.back().nextAvailableObject = m_nextAvailableObject;
}

Chunk& ChunkPool::getChunk(const glm::ivec3& startingPosition)
{
	int iterationCount = 0;
	bool validChunkFound = false;

	while (!validChunkFound)
	{
		assert(m_nextAvailableObject);
		if (m_nextAvailableObject->object.isInUse())
		{
			assert(m_nextAvailableObject);
			//m_nextAvailableObject = m_nextAvailableObjec->nextChunkInPool;
			m_nextAvailableObject = m_nextAvailableObject->nextAvailableObject;// nextChunkInPool;
		}
		else
		{
			validChunkFound = true;
		}

		assert(++iterationCount && iterationCount <= m_objectPool.size());
	}
	
	m_nextAvailableObject->object.reuse(startingPosition);
	return m_nextAvailableObject->object;
}