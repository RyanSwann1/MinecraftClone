#pragma once

#include "ObjectPool.h"
#include "Chunk.h"

class ChunkPool : private ObjectPool<Chunk>
{
public:
	ChunkPool();

	Chunk& getChunk(const glm::ivec3& startingPosition);
};

struct ChunkFromPool : public ObjectFromPool<Chunk, ChunkPool>
{
	ChunkFromPool(ChunkPool& chunkPool, const glm::ivec3& startingPosition);
	~ChunkFromPool();
};
