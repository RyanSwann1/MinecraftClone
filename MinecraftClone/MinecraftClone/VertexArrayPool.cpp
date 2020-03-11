#include "VertexArrayPool.h"
#include "Utilities.h"

VertexArrayPool::VertexArrayPool()
	: m_vertexArrayPool(),
	m_nextAvailable(nullptr)
{
	int x = Utilities::VISIBILITY_DISTANCE / Utilities::CHUNK_WIDTH;
	x += x += 2;
	int y = Utilities::VISIBILITY_DISTANCE / Utilities::CHUNK_DEPTH;
	y += y += 2;

	m_vertexArrayPool.resize(size_t((x * y)));
	
	for (int i = 0; i < static_cast<int>(m_vertexArrayPool.size()) - 1; ++i)
	{
		m_vertexArrayPool[i].m_next = &m_vertexArrayPool[i + 1];
	}

	m_nextAvailable = &m_vertexArrayPool.front();
	m_vertexArrayPool.back().m_next = m_nextAvailable;
}

VertexArray& VertexArrayPool::getVertexArray()
{
	int iterationCount = 0;
	bool validVertexArrayFound = false;

	while (!validVertexArrayFound)
	{
		assert(m_nextAvailable);
		if (m_nextAvailable->m_inUse)
		{
			assert(m_nextAvailable->m_next);
			m_nextAvailable = m_nextAvailable->m_next;
		}
		else
		{
			validVertexArrayFound = true;
		}

		assert(++iterationCount && iterationCount <= m_vertexArrayPool.size());
	}

	assert(m_nextAvailable);
	m_nextAvailable->m_inUse = true;
	return *m_nextAvailable;
}