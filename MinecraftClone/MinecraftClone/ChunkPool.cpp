#include "ChunkPool.h"

ChunkPool::ChunkPool()
{
	m_chunks.resize(size_t(2500));
	
	m_firstAvailable = &m_chunks.front();

	for (int i = 0; i < m_chunks.size() - 1; ++i)
	{
		m_chunks[i].setNext(&m_chunks[i + 1]);
	}

	m_chunks.back().setNext(nullptr);
}

Chunk& ChunkPool::getChunk(const glm::ivec3& startingPosition)
{
	Chunk* chunk = m_firstAvailable;
	m_firstAvailable = chunk->getNext();

	return *chunk;

	//for (auto& chunk : m_chunks)
	//{
	//	if (!chunk.isInUse())
	//	{
	//		chunk.reuse(startingPosition);
	//		return chunk;
	//	}
	//}
}

void ChunkPool::releaseChunk(Chunk& chunk)
{
	chunk.setNext(m_firstAvailable);
	m_firstAvailable = &chunk;
}