#pragma once

#include "ObjectQueue.h"
#include "ObjectPool.h"
#include <unordered_map>

class Chunk;
struct VertexArray;

struct GeneratedChunkMeshQueue;
struct ChunkMeshesToGenerateQueue : public ObjectQueue<PositionNode>
{
	ChunkMeshesToGenerateQueue();

	void update(ObjectPool<VertexArray>& chunkMeshPool, const std::unordered_map<glm::ivec3, ObjectFromPool<Chunk>>& chunks, 
		GeneratedChunkMeshQueue& generatedChunkMeshQueue);
};