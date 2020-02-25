#pragma once

#include "VertexBuffer.h"
#include "NonCopyable.h"
#include "glm/glm.hpp"

struct VertexArray : private NonCopyable
{
	VertexArray();
	~VertexArray();
	
	void reset();
	void init() const;
	void bind() const;
	void unbind() const;

	VertexBuffer m_vertexBuffer;
	unsigned int m_ID;
};