#include "ChunkPool.h"
#include <iostream>

//ChunkFromPool
ChunkFromPool::ChunkFromPool(ChunkPool& chunkPool, const glm::ivec3& startingPosition)
	: ObjectFromPool(chunkPool.getChunk(startingPosition))
{}

ChunkFromPool::ChunkFromPool(ChunkFromPool&& orig) noexcept
	: ObjectFromPool(std::move(orig))
{}

ChunkFromPool& ChunkFromPool::operator=(ChunkFromPool&& orig) noexcept
{
	ObjectFromPool::operator=(std::move(orig));
	return *this;
}

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

Chunk* ChunkPool::getChunk(const glm::ivec3& startingPosition)
{
	Chunk* chunk = getNextAvailableObject();
	if (chunk)
	{
		assert(!chunk->isInUse());
		chunk->reuse(startingPosition);
	}

	return chunk;
}