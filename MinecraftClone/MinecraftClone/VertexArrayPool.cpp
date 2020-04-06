#include "VertexArrayPool.h"
#include "Utilities.h"

//Vertex Array Pool
VertexArrayPool::VertexArrayPool()
	: ObjectPool()
{}

VertexArray* VertexArrayPool::getVertexArray()
{
	VertexArray* vertexArray = getNextAvailableObject();
	if (vertexArray)
	{
		assert(!vertexArray->isInUse());
		vertexArray->m_inUse = true;
	}

	return vertexArray;
}

//VertexArrayFromPool
VertexArrayFromPool::VertexArrayFromPool(VertexArrayPool& vertexArrayPool)
	: ObjectFromPool(vertexArrayPool.getVertexArray())
{}

VertexArrayFromPool::VertexArrayFromPool(VertexArrayFromPool && orig) noexcept
	: ObjectFromPool(std::move(orig))
{}

VertexArrayFromPool& VertexArrayFromPool::operator=(VertexArrayFromPool&& orig) noexcept
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