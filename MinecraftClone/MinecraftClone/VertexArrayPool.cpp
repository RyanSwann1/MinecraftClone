#include "VertexArrayPool.h"
#include "Utilities.h"

VertexArrayPool::VertexArrayPool()
	: ObjectPool()
{
	int x = Utilities::VISIBILITY_DISTANCE / Utilities::CHUNK_WIDTH;
	x += x += 2;
	int y = Utilities::VISIBILITY_DISTANCE / Utilities::CHUNK_DEPTH;
	y += y += 2;

	m_objectPool.resize(size_t((x * y)));
	
	for (int i = 0; i < static_cast<int>(m_objectPool.size()) - 1; ++i)
	{
		m_objectPool[i].nextAvailableObject = &m_objectPool[i + 1];
	}

	m_nextAvailableObject = &m_objectPool.front();
	m_objectPool.back().nextAvailableObject = m_nextAvailableObject;
}

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
	object.m_inUse = false;
	object.m_opaqueVBODisplayable = false;
	object.m_transparentVBODisplayable = false;
}