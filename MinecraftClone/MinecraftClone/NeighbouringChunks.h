#pragma once

#include "NonCopyable.h"
#include "Globals.h"
#include "Chunk.h"
#include <unordered_map>
#include "ObjectPool.h"
#include "glm/gtx/hash.hpp"

bool isAllNeighbouringChunksAvailable(const std::unordered_map<glm::ivec3, ObjectFromPool<Chunk>>& chunks,
	const glm::ivec3& middleChunkStartingPosition);

NeighbouringChunks getAllNeighbouringChunks(const std::unordered_map<glm::ivec3, ObjectFromPool<Chunk>>& chunks,
	const glm::ivec3& middleChunkStartingPosition);

struct NeighbouringChunks : private NonCopyable
{
	NeighbouringChunks(const Chunk& leftChunk, const Chunk& rightChunk,
		const Chunk& forwardChunk, const Chunk& backChunk);
	NeighbouringChunks(NeighbouringChunks&&) noexcept;
	NeighbouringChunks& operator=(NeighbouringChunks&&) noexcept;

	std::array<std::reference_wrapper<const Chunk>,
		static_cast<size_t>(eDirection::Max) + 1> chunks;
};