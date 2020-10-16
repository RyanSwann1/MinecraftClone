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
			if (chunkMeshPool.isObjectAvailable())
			{
				ObjectFromPool<VertexArray> chunkMeshFromPool = chunkMeshPool.getAvailableObject();
				auto chunk = chunks.find(chunkStartingPosition);
				if (chunk != chunks.cend())
				{
					MeshGenerator::generateChunkMesh(chunkMeshFromPool.object, chunk->second.object,
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
		else
		{
			chunkMeshToGenerate = next(chunkMeshToGenerate);
		}
	}
}
