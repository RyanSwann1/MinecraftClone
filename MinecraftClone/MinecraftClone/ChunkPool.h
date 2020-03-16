#pragma once

#include "ObjectPool.h"
#include "Chunk.h"


//template <class Object>
//struct ObjectInPool : private NonCopyable
//{
//	ObjectInPool() {}
//	virtual ~ObjectInPool() {}
//
//	Object object;
//	ObjectInPool* objectInPool;
//};
//
//template <class Object, class ObjectPool>
//struct ObjectFromPool : private NonCopyable, private NonMovable
//{
//	ObjectFromPool(Object& object, ObjectPool& objectPool)
//		: objectPool(objectPool),
//		object(object)
//	{}
//	virtual ~ObjectFromPool() {}
//
//	ObjectPool& objectPool;
//	Object& object;
//};
//
//template <class Object>
//class ObjectPool : private NonCopyable, private NonMovable
//{
//protected:
//	std::vector<Object> m_objectPool;
//	Object* m_nextAvailable;
//};

class ChunkPool : private ObjectPool<Chunk>
{
public:
	ChunkPool();

	Chunk& getChunk(const glm::ivec3& startingPosition);
};

struct ChunkFromPool : public ObjectFromPool<Chunk, ChunkPool>
{
	ChunkFromPool(ChunkPool& chunkPool, const glm::ivec3& startingPosition);
	~ChunkFromPool();
};

////ChunkFromPool
//ChunkFromPool::ChunkFromPool(ChunkPool& chunkPool, const glm::ivec3& startingPosition)
//	: chunkPool(chunkPool),
//	chunk(chunkPool.getChunk(startingPosition))
//{}
//
//ChunkFromPool::~ChunkFromPool()
//{
//	chunk.release();
//}
//
////ChunkInPool
//ChunkInPool::ChunkInPool()
//	: chunk(),
//	nextChunkInPool(nullptr)
//{}
//
//ChunkInPool::ChunkInPool(ChunkInPool&& orig) noexcept
//	: chunk(std::move(orig.chunk)),
//	nextChunkInPool(orig.nextChunkInPool)
//{
//	orig.nextChunkInPool = nullptr;
//}
//
//ChunkInPool& ChunkInPool::operator=(ChunkInPool&& orig) noexcept
//{
//	chunk = std::move(orig.chunk);
//	nextChunkInPool = orig.nextChunkInPool;
//
//	orig.nextChunkInPool = nullptr;
//
//	return *this;
//}



















//
////External use
//class ChunkPool;
//struct ChunkFromPool : private NonCopyable, private NonMovable
//{
//	ChunkFromPool(ChunkPool& chunkPool, const glm::ivec3& startingPosition);
//	~ChunkFromPool();
//
//	ChunkPool& chunkPool;
//	Chunk& chunk;
//};
//
////Internal use
//struct ChunkInPool : private NonCopyable
//{
//	ChunkInPool();
//	ChunkInPool(ChunkInPool&&) noexcept;
//	ChunkInPool& operator=(ChunkInPool&&) noexcept;
//
//	Chunk chunk;
//	ChunkInPool* nextChunkInPool;
//};
//
//class ChunkPool : private NonCopyable, private NonMovable
//{
//public:
//	ChunkPool();
//
//	Chunk& getChunk(const glm::ivec3& startingPosition);
//
//private:
//	std::vector<ChunkInPool> m_chunks;
//	ChunkInPool* m_nextAvailable;
//};