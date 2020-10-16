#include "GeneratedChunkQueue.h"

//GeneratedChunk
GeneratedChunk::GeneratedChunk(const glm::ivec3& position, ObjectFromPool<Chunk>&& chunkFromPool)
	: ObjectQueueNode(position),
	chunkFromPool(std::move(chunkFromPool))
{}

GeneratedChunk::GeneratedChunk(GeneratedChunk&& orig) noexcept
	: ObjectQueueNode(std::move(orig)),
	chunkFromPool(std::move(orig.chunkFromPool))
{}

GeneratedChunk& GeneratedChunk::operator=(GeneratedChunk&& orig) noexcept
{
	ObjectQueueNode::operator=(std::move(orig));
	chunkFromPool = std::move(orig.chunkFromPool);

	return *this;
}

//GeneratedChunkQueue
GeneratedChunkQueue::GeneratedChunkQueue()
	: ObjectQueue()
{}

void GeneratedChunkQueue::update(std::unordered_map<glm::ivec3, ObjectFromPool<Chunk>>& chunks)
{
	if (!isEmpty())
	{
		GeneratedChunk& generatedChunk = front();

		chunks.emplace(std::piecewise_construct,
			std::forward_as_tuple(generatedChunk.getPosition()),
			std::forward_as_tuple(std::move(generatedChunk.chunkFromPool)));

		pop();
	}
}