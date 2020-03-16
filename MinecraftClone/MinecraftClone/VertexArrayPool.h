#pragma once

#include "VertexArray.h"
#include "ObjectPool.h"

//template <class T>
//class ObjectPool : private NonCopyable, private NonMovable
//{
//protected:
//	std::vector<T> m_objectPool;
//	T* m_nextAvailable;
//};
//
//class VertexArrayPool : private ObjectPool<VertexArray>
//{
//public:
//	VertexArray& getVertexArray();
//};
//
//class Chunk;
//class ChunkPool : private ObjectPool<Chunk>
//{
//public:
//	Chunk& getChunk(const glm::ivec3& startingPosition);
//};

class VertexArrayPool : private ObjectPool<VertexArray>
{
public:
	VertexArrayPool();

	VertexArray& getVertexArray();
};

struct VertexArrayFromPool : public ObjectFromPool<VertexArray, VertexArrayPool>
{
	VertexArrayFromPool(VertexArrayPool& vertexArrayPool);
	~VertexArrayFromPool();
};