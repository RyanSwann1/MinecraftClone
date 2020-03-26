#include "ChunkPool.h"
#include <iostream>

//ChunkFromPool
ChunkFromPool::ChunkFromPool(ChunkPool& chunkPool, const glm::ivec3& startingPosition)
	: ObjectFromPool(chunkPool.getChunk(startingPosition))
{}

ChunkFromPool::~ChunkFromPool()
{
	if (object)
	{
		object->release();
	}
}

//ChunkPool
ChunkPool::ChunkPool()
	: ObjectPool()
{}

Chunk& ChunkPool::getChunk(const glm::ivec3& startingPosition)
{
	ObjectInPool<Chunk>& nextAvailableChunk = getNextAvailableObject();
	nextAvailableChunk.object.reuse(startingPosition);

	return nextAvailableChunk.object;
}