#pragma once

#include "VertexArray.h"
#include "NonMovable.h"
#include "NonCopyable.h"

class VertexArrayPool : private NonCopyable, private NonMovable
{
public:
	VertexArrayPool();

	VertexArray& getVertexArray();

private:
	std::vector<VertexArray> m_vertexArrayPool;
	VertexArray* m_nextAvailable;
};

struct VertexArrayFromPool : private NonCopyable, private NonMovable
{
	VertexArrayFromPool(VertexArrayPool& vertexArrayPool)
		: vertexArray(vertexArrayPool.getVertexArray())
	{}

	~VertexArrayFromPool()
	{
		vertexArray.m_inUse = false;
		vertexArray.m_opaqueVBODisplayable = false;
		vertexArray.m_transparentVBODisplayable = false;
	}

	VertexArray& vertexArray;
};