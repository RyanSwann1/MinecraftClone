#include "VertexArrayPool.h"
#include "Utilities.h"

//Vertex Array Pool
VertexArrayPool::VertexArrayPool()
	: ObjectPool()
{}

VertexArray& VertexArrayPool::getVertexArray()
{
	ObjectInPool<VertexArray>& nextAvailableObject = getNextAvailableObject();
	nextAvailableObject.object.m_inUse = true;

	return nextAvailableObject.object;
}

//VertexArrayFromPool
VertexArrayFromPool::VertexArrayFromPool(VertexArrayPool& vertexArrayPool)
	: ObjectFromPool(vertexArrayPool.getVertexArray())
{}

VertexArrayFromPool::~VertexArrayFromPool()
{
	if (object)
	{
		object->reset();
	}
}