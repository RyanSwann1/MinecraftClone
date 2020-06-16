#pragma once

#include "ObjectQueue.h"
#include "ObjectPool.h"
#include "VertexArray.h"

struct GeneratedChunkMesh : public ObjectQueueNode<GeneratedChunkMesh>
{
	GeneratedChunkMesh(const glm::ivec3& position, ObjectFromPool<VertexArray>&& chunkMeshFromPool);
	GeneratedChunkMesh(GeneratedChunkMesh&&) noexcept;
	GeneratedChunkMesh& operator=(GeneratedChunkMesh&&) noexcept;

	ObjectFromPool<VertexArray> chunkMeshFromPool;
};

struct GeneratedChunkMeshQueue : ObjectQueue<GeneratedChunkMesh>
{
	GeneratedChunkMeshQueue();

	void update(std::unordered_map<glm::ivec3, ObjectFromPool<VertexArray>>& chunkMeshes);
};