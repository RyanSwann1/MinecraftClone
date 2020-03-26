#pragma once

#include "VertexArray.h"
#include "ObjectPool.h"

class VertexArrayPool : private ObjectPool<VertexArray>
{
public:
	VertexArrayPool();

	VertexArray& getVertexArray();
};

struct VertexArrayFromPool : public ObjectFromPool<VertexArray, VertexArrayPool>
{
	VertexArrayFromPool(VertexArrayPool& vertexArrayPool);
	VertexArrayFromPool(VertexArrayFromPool&& orig);
	VertexArrayFromPool& operator=(VertexArrayFromPool&& orig);
	~VertexArrayFromPool();
};