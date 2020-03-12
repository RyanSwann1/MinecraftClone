#pragma once

#include "NonMovable.h"
#include "Chunk.h"
#include <vector>

//External use
class ChunkPool;
struct ChunkFromPool : private NonCopyable, private NonMovable
{
	ChunkFromPool(ChunkPool& chunkPool, const glm::ivec3& startingPosition);
	~ChunkFromPool();

	ChunkPool& chunkPool;
	Chunk& chunk;
};

//Internal use
struct ChunkInPool : private NonCopyable
{
	ChunkInPool();
	ChunkInPool(ChunkInPool&&) noexcept;
	ChunkInPool& operator=(ChunkInPool&&) noexcept;

	Chunk chunk;
	ChunkInPool* nextChunkInPool;
};

class ChunkPool : private NonCopyable, private NonMovable
{
public:
	ChunkPool();

	Chunk& getChunk(const glm::ivec3& startingPosition);

private:
	std::vector<ChunkInPool> m_chunks;
	ChunkInPool* m_nextAvailable;
};