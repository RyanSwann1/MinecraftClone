#include "ChunkMeshRegenerationQueue.h"
#include "NeighbouringChunks.h"
#include "MeshGenerator.h"

//ChunkMeshToRegenerate
ChunkMeshToRegenerate::ChunkMeshToRegenerate(const glm::ivec3& position, VertexArray& chunkMeshToRegenerate)
	: ObjectQueueNode(position),
	chunkMeshToRegenerate(chunkMeshToRegenerate)
{}

ChunkMeshToRegenerate::ChunkMeshToRegenerate(ChunkMeshToRegenerate&& orig) noexcept
	: ObjectQueueNode(std::move(orig)),
	chunkMeshToRegenerate(orig.chunkMeshToRegenerate)
{}

ChunkMeshToRegenerate& ChunkMeshToRegenerate::operator=(ChunkMeshToRegenerate&& orig) noexcept
{
	ObjectQueueNode::operator=(std::move(orig));
	chunkMeshToRegenerate = orig.chunkMeshToRegenerate;

	return *this;
}

//ChunkMeshRegenerationQueue
ChunkMeshRegenerationQueue::ChunkMeshRegenerationQueue()
	: ObjectQueue()
{}

void ChunkMeshRegenerationQueue::update(const std::unordered_map<glm::ivec3, ObjectFromPool<Chunk>>& chunks)
{
	if (!isEmpty())
	{
		ChunkMeshToRegenerate* regenNode = &front();
		while (regenNode)
		{
			regenNode->chunkMeshToRegenerate.get().reset();

			const glm::ivec3& chunkStartingPosition = regenNode->getPosition();

			auto chunk = chunks.find(chunkStartingPosition);
			assert(chunk != chunks.cend());

			generateChunkMesh(regenNode->chunkMeshToRegenerate.get(), *chunk->second.getObject(),
				getAllNeighbouringChunks(chunks, chunkStartingPosition));

			regenNode = remove(regenNode);
		}
	}
}