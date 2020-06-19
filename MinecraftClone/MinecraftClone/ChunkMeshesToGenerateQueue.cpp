#include "ChunkMeshesToGenerateQueue.h"
#include "NeighbouringChunks.h"
#include "MeshGenerator.h"
#include "GeneratedChunkMeshQueue.h"
#include "Chunk.h"
#include <assert.h>

ChunkMeshesToGenerateQueue::ChunkMeshesToGenerateQueue()
	: ObjectQueue()
{}

void ChunkMeshesToGenerateQueue::update(ObjectPool<VertexArray>& chunkMeshPool, const std::unordered_map<glm::ivec3, ObjectFromPool<Chunk>>& chunks, 
	GeneratedChunkMeshQueue& generatedChunkMeshQueue)
{
	if (isEmpty())
	{
		return;
	}

	PositionNode* chunkMeshToGenerate = &front();
	while (chunkMeshToGenerate)
	{
		const glm::ivec3& chunkStartingPosition = chunkMeshToGenerate->getPosition();
		if (isAllNeighbouringChunksAvailable(chunks, chunkStartingPosition))
		{
			ObjectFromPool<VertexArray> chunkMeshFromPool = chunkMeshPool.getNextAvailableObject();
			if (chunkMeshFromPool.getObject())
			{
				auto chunk = chunks.find(chunkStartingPosition);
				assert(chunk != chunks.cend());

				MeshGenerator::generateChunkMesh(*chunkMeshFromPool.getObject(), *chunk->second.getObject(),
					getAllNeighbouringChunks(chunks, chunkStartingPosition));

				generatedChunkMeshQueue.add({ chunkStartingPosition, std::move(chunkMeshFromPool) });
				chunkMeshToGenerate = remove(chunkMeshToGenerate);
			}
			else
			{
				chunkMeshToGenerate = next(chunkMeshToGenerate);
			}
		}
		else
		{
			chunkMeshToGenerate = next(chunkMeshToGenerate);
		}
	}
}
