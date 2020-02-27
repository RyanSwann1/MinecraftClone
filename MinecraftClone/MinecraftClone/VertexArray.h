#pragma once

#include "VertexBuffer.h"
#include "NonCopyable.h"
#include "glm/glm.hpp"

struct VertexArray : private NonCopyable
{
	VertexArray();
	
	void reset();
	void destroy();

	void attachTransparentVBO();
	void attachOpaqueVBO();
	void bind() const;
	void unbind() const;

	bool m_displayable;
	bool m_attachOpaqueVBO;
	bool m_attachTransparentVBO;
	bool m_destroy;
	VertexBuffer m_vertexBuffer;
	unsigned int m_ID;
};