#pragma once

#include "NonMovable.h"
#include "NonCopyable.h"
#include <vector>

template <class Object>
struct ObjectInPool : private NonCopyable
{
	ObjectInPool() {}
	virtual ~ObjectInPool() {}

	Object object;
	ObjectInPool* objectInPool;
};

template <class Object, class ObjectPool>
struct ObjectFromPool : private NonCopyable, private NonMovable
{
	ObjectFromPool() {}
	virtual ~ObjectFromPool() {}

	ObjectPool& objectPool;
	Object& object;
};

template <class Object>
class ObjectPool : private NonCopyable, private NonMovable
{
protected:
	std::vector<Object> m_objectPool;
	Object* m_nextAvailable;
};

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

//
//class VertexArrayPool : private NonCopyable, private NonMovable
//{
//public:
//	VertexArrayPool();
//
//	VertexArray& getVertexArray();
//
//private:
//	std::vector<VertexArray> m_vertexArrayPool;
//	VertexArray* m_nextAvailable;
//};
//
//struct VertexArrayFromPool : private NonCopyable, private NonMovable
//{
//	VertexArrayFromPool(VertexArrayPool& vertexArrayPool)
//		: vertexArray(vertexArrayPool.getVertexArray())
//	{}
//
//	~VertexArrayFromPool()
//	{
//		vertexArray.m_inUse = false;
//		vertexArray.m_opaqueVBODisplayable = false;
//		vertexArray.m_transparentVBODisplayable = false;
//	}
//
//	VertexArray& vertexArray;
//};