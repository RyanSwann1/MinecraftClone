#pragma once

#include "VertexBuffer.h"
#include "NonCopyable.h"
#include "glm/glm.hpp"

struct VertexArray : private NonCopyable
{
	VertexArray();
	~VertexArray();
	VertexArray(VertexArray&&) noexcept;
	VertexArray& operator=(VertexArray&&) noexcept;
	
	void reset();

	void attachTransparentVBO();
	void attachOpaqueVBO();
	void bindOpaqueVAO() const;
	void bindTransparentVAO() const;
	void unbind() const;
	
	VertexBuffer m_opaqueVertexBuffer;
	VertexBuffer m_transparentVertexBuffer;
	unsigned int m_opaqueID;
	unsigned int m_transparentID;
};