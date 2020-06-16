#pragma once

#include "ObjectQueue.h"
#include "ObjectPool.h"
#include "Chunk.h"

struct GeneratedChunk : public ObjectQueueNode<GeneratedChunk>
{
	GeneratedChunk(const glm::ivec3& position, ObjectFromPool<Chunk>&& chunkFromPool);
	GeneratedChunk(GeneratedChunk&&) noexcept;
	GeneratedChunk& operator=(GeneratedChunk&&) noexcept;

	ObjectFromPool<Chunk> chunkFromPool;
};

struct GeneratedChunkQueue : public ObjectQueue<GeneratedChunk>
{
	GeneratedChunkQueue();

	void update(std::unordered_map<glm::ivec3, ObjectFromPool<Chunk>>& chunks);
};