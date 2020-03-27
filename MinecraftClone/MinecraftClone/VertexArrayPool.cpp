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

VertexArrayFromPool::VertexArrayFromPool(VertexArrayFromPool && orig)
	: ObjectFromPool(std::move(orig))
{
	int i = 0;
}

VertexArrayFromPool& VertexArrayFromPool::operator=(VertexArrayFromPool&& orig)
{
	ObjectFromPool::operator=(std::move(orig));
	return *this;
}

VertexArrayFromPool::~VertexArrayFromPool()
{
	if (object)
	{
		object->reset();
	}
}