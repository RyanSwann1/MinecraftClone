#include "NeighbouringChunks.h"
#include "ObjectPool.h"

glm::ivec3 getNeighbouringChunkPosition(const glm::ivec3& chunkStartingPosition, eDirection direction)
{
	switch (direction)
	{
	case eDirection::Left:
		return glm::ivec3(chunkStartingPosition.x - Globals::CHUNK_WIDTH, 0, chunkStartingPosition.z);

	case eDirection::Right:
		return glm::ivec3(chunkStartingPosition.x + Globals::CHUNK_WIDTH, 0, chunkStartingPosition.z);

	case eDirection::Forward:
		return glm::ivec3(chunkStartingPosition.x, 0, chunkStartingPosition.z + Globals::CHUNK_DEPTH);

	case eDirection::Back:
		return glm::ivec3(chunkStartingPosition.x, 0, chunkStartingPosition.z - Globals::CHUNK_DEPTH);

	default:
		assert(false);
	}
}

bool isAllNeighbouringChunksAvailable(const std::unordered_map<glm::ivec3, ObjectFromPool<Chunk>>& chunks, const glm::ivec3& middleChunkStartingPosition)
{
	auto leftChunk = chunks.find(getNeighbouringChunkPosition(middleChunkStartingPosition, eDirection::Left));
	auto rightChunk = chunks.find(getNeighbouringChunkPosition(middleChunkStartingPosition, eDirection::Right));
	auto forwardChunk = chunks.find(getNeighbouringChunkPosition(middleChunkStartingPosition, eDirection::Forward));
	auto backChunk = chunks.find(getNeighbouringChunkPosition(middleChunkStartingPosition, eDirection::Back));

	return (leftChunk != chunks.cend() &&
		rightChunk != chunks.cend() &&
		forwardChunk != chunks.cend() &&
		backChunk != chunks.cend());
}

NeighbouringChunks getAllNeighbouringChunks(const std::unordered_map<glm::ivec3, ObjectFromPool<Chunk>>& chunks, const glm::ivec3& middleChunkStartingPosition)
{
	auto leftChunk = chunks.find(getNeighbouringChunkPosition(middleChunkStartingPosition, eDirection::Left));
	auto rightChunk = chunks.find(getNeighbouringChunkPosition(middleChunkStartingPosition, eDirection::Right));
	auto forwardChunk = chunks.find(getNeighbouringChunkPosition(middleChunkStartingPosition, eDirection::Forward));
	auto backChunk = chunks.find(getNeighbouringChunkPosition(middleChunkStartingPosition, eDirection::Back));

	assert(leftChunk != chunks.cend() &&
		rightChunk != chunks.cend() &&
		forwardChunk != chunks.cend() &&
		backChunk != chunks.cend());

	return NeighbouringChunks(leftChunk->second.object, rightChunk->second.object,
		forwardChunk->second.object, backChunk->second.object);
}

//NeighbouringChunks
NeighbouringChunks::NeighbouringChunks(const Chunk& leftChunk, const Chunk& rightChunk, const Chunk& forwardChunk, const Chunk& backChunk)
	: chunks{ leftChunk, rightChunk, forwardChunk, backChunk }
{}

NeighbouringChunks::NeighbouringChunks(NeighbouringChunks&& orig) noexcept
	: chunks(std::move(orig.chunks))
{}

NeighbouringChunks& NeighbouringChunks::operator=(NeighbouringChunks&& orig) noexcept
{
	chunks = std::move(orig.chunks);
	return *this;
}