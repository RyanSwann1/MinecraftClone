#pragma once

#include "ObjectPool.h"
#include "Chunk.h"

struct ChunkPool;
struct ChunkFromPool : public ObjectFromPool<Chunk, ChunkPool>
{
	ChunkFromPool(ChunkPool& chunkPool, const glm::ivec3& startingPosition);
	ChunkFromPool(ChunkFromPool&&) noexcept;
	ChunkFromPool& operator=(ChunkFromPool&&) noexcept;
	~ChunkFromPool();
};

struct ChunkPool : private ObjectPool<Chunk>
{
	ChunkPool();

	Chunk* getChunk(const glm::ivec3& startingPosition);
};