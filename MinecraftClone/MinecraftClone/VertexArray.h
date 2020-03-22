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
	
	bool isInUse() const;
	void reset();

	void attachTransparentVBO();
	void attachOpaqueVBO();
	void bindOpaqueVAO() const;
	void bindTransparentVAO() const;
	void unbind() const;
	
	bool m_inUse;
	bool m_opaqueVBODisplayable;
	bool m_transparentVBODisplayable;
	bool m_attachOpaqueVBO;
	bool m_attachTransparentVBO;
	bool m_awaitingRegeneration;
	VertexBuffer m_vertexBuffer;
	unsigned int m_ID;
	unsigned int m_transparentID;
	int m_opaqueElementBufferIndex;
	int m_transparentElementBufferIndex;
};