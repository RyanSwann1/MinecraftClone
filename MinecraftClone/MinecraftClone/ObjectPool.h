#pragma once

#include "NonMovable.h"
#include "NonCopyable.h"
#include <vector>

//Internal Use - Object Pool
template <class Object>
struct ObjectInPool : private NonCopyable
{
	ObjectInPool()
		: object(),
		nextAvailableObject()
	{}
	ObjectInPool(ObjectInPool&& orig) noexcept
		: object(std::move(orig)),
		nextAvailableObject(orig.nextAvailable)
	{
		orig.nextAvailableObject = nullptr;
	}
	ObjectInPool& operator=(ObjectInPool&& orig) noexcept
	{
		object = std::move(orig.object);
		nextAvailableObject = orig.nextAvailableObject;
		orig.nextAvailableObject = nullptr;

		return *this;
	}
	virtual ~ObjectInPool() {}

	Object object;
	ObjectInPool* nextAvailableObject;
};

//External Use - Object Pool
template <class Object, class ObjectPool>
struct ObjectFromPool : private NonCopyable, private NonMovable
{
	ObjectFromPool(Object& object, ObjectPool& objectPool) 
		: objectPool(objectPool),
		object(object)
	{}
	virtual ~ObjectFromPool() {}

	ObjectPool& objectPool;
	Object& object;
};

//Object Pool
template <class Object>
class ObjectPool : private NonCopyable, private NonMovable
{
public:
	ObjectPool()
		: m_objectPool(),
		m_nextAvailableObject(nullptr)
	{}
	virtual ~ObjectPool() {}

protected:
	std::vector<ObjectInPool<Object>> m_objectPool;
	ObjectInPool<Object>* m_nextAvailableObject;
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
