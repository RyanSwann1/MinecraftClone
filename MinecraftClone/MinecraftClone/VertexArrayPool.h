#pragma once

#include "VertexArray.h"
#include "NonCopyable.h"
#include <array>

class VertexArrayPool : private NonCopyable
{
public:
	VertexArrayPool();

	VertexArray& getVertexArray();

private:
	std::vector<VertexArray> m_vertexArrayPool;
	VertexArray* m_nextAvailable;
};

struct VertexArrayFromPool : private NonCopyable
{
	VertexArrayFromPool(VertexArrayPool& vertexArrayPool)
		: vertexArray(vertexArrayPool.getVertexArray())
	{}

	~VertexArrayFromPool()
	{
		vertexArray.m_inUse = false;
	}

	VertexArray& vertexArray;
};