#include "VertexArrayPool.h"
#include "Utilities.h"

VertexArrayPool::VertexArrayPool()
	: ObjectPool()
{}

VertexArray& VertexArrayPool::getVertexArray()
{
	int iterationCount = 0;
	bool validVertexArrayFound = false;

	while (!validVertexArrayFound)
	{
		assert(m_nextAvailableObject);
		if (m_nextAvailableObject->object.m_inUse)
		{
			assert(m_nextAvailableObject->nextAvailableObject);
			m_nextAvailableObject = m_nextAvailableObject->nextAvailableObject;
		}
		else
		{
			validVertexArrayFound = true;
		}

		assert(++iterationCount && iterationCount <= m_objectPool.size());
	}

	m_nextAvailableObject->object.m_inUse = true;
	return m_nextAvailableObject->object;
}

VertexArrayFromPool::VertexArrayFromPool(VertexArrayPool& vertexArrayPool)
	: ObjectFromPool(vertexArrayPool.getVertexArray(), vertexArrayPool)
{}

VertexArrayFromPool::~VertexArrayFromPool()
{
	object.reset();
}