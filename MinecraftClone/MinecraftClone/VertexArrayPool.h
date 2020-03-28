#pragma once

#include "VertexArray.h"
#include "ObjectPool.h"

struct VertexArrayPool : private ObjectPool<VertexArray>
{
	VertexArrayPool();

	VertexArray& getVertexArray();
};

struct VertexArrayFromPool : public ObjectFromPool<VertexArray, VertexArrayPool>
{
	VertexArrayFromPool(VertexArrayPool& vertexArrayPool);
	VertexArrayFromPool(VertexArrayFromPool&& orig) noexcept;
	VertexArrayFromPool& operator=(VertexArrayFromPool&& orig) noexcept;
	~VertexArrayFromPool();
};