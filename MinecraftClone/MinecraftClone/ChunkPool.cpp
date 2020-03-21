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
	: ObjectPool()
{}

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
			m_nextAvailableObject = m_nextAvailableObject->nextAvailableObject;
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