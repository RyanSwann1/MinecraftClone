#include "GeneratedChunkMeshQueue.h"

//GeneratedChunkMesh
GeneratedChunkMesh::GeneratedChunkMesh(const glm::ivec3& position, ObjectFromPool<VertexArray>&& chunkMeshFromPool)
	: ObjectQueueNode(position),
	chunkMeshFromPool(std::move(chunkMeshFromPool))
{}

GeneratedChunkMesh::GeneratedChunkMesh(GeneratedChunkMesh&& orig) noexcept
	: ObjectQueueNode(std::move(orig)),
	chunkMeshFromPool(std::move(orig.chunkMeshFromPool))
{}

GeneratedChunkMesh& GeneratedChunkMesh::operator=(GeneratedChunkMesh&& orig) noexcept
{
	ObjectQueueNode::operator=(std::move(orig));
	chunkMeshFromPool = std::move(orig.chunkMeshFromPool);

	return *this;
}

//GeneratedChunkMeshQueue
GeneratedChunkMeshQueue::GeneratedChunkMeshQueue()
	: ObjectQueue()
{}

void GeneratedChunkMeshQueue::update(std::unordered_map<glm::ivec3, ObjectFromPool<VertexArray>>& chunkMeshes)
{
	if (!isEmpty())
	{
		GeneratedChunkMesh& generatedChunkMesh = front();
		assert(generatedChunkMesh.chunkMeshFromPool.getObject());

		chunkMeshes.emplace(std::piecewise_construct,
			std::forward_as_tuple(generatedChunkMesh.getPosition()),
			std::forward_as_tuple(std::move(generatedChunkMesh.chunkMeshFromPool)));

		pop();
	}
}
