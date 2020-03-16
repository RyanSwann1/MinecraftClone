#pragma once

#include "VertexArray.h"
#include "ObjectPool.h"


template <class T>
class ObjectPool : private NonCopyable, private NonMovable
{
public:


protected:
	std::vector<T> m_objectPool;
	T* m_nextAvailable;
};

class VertexArrayPool : private ObjectPool<VertexArray>
{
public:
	VertexArray& getVertexArray();
};

class Chunk;
class ChunkPool : private ObjectPool<Chunk>
{
public:
	Chunk& getChunk(const glm::ivec3& startingPosition);
};

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