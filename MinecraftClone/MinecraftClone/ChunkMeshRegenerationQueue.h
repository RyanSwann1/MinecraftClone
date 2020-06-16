#pragma once

#include "VertexArray.h"
#include "ObjectQueue.h"
#include <unordered_map>

struct ChunkMeshToRegenerate : public ObjectQueueNode<ChunkMeshToRegenerate>
{
	ChunkMeshToRegenerate(const glm::ivec3& position, VertexArray& chunkMeshToRegenerate);
	ChunkMeshToRegenerate(ChunkMeshToRegenerate&&) noexcept;
	ChunkMeshToRegenerate& operator=(ChunkMeshToRegenerate&&) noexcept;

	std::reference_wrapper<VertexArray> chunkMeshToRegenerate;
};

class Chunk;
template <typename Chunk>
struct ObjectFromPool;
struct ChunkMeshRegenerationQueue : public ObjectQueue<ChunkMeshToRegenerate>
{
	ChunkMeshRegenerationQueue();

	void update(const std::unordered_map<glm::ivec3, ObjectFromPool<Chunk>>& chunks);
};