#pragma once

#include "VertexBuffer.h"
#include "NonCopyable.h"
#include "glm/glm.hpp"
#include <iostream>

struct VertexArray //: private NonCopyable
{
	VertexArray();
	VertexArray(const VertexArray&)
	{
		std::cout << "Copy Constructor\n";
	}
	VertexArray& operator=(const VertexArray&)
	{
		std::cout << "Copy Assignment Operator\n";
	}
	VertexArray(VertexArray&&)
	{
		std::cout << "Move Constructor\n";
	}
	VertexArray&& operator=(VertexArray&&)
	{
		std::cout << "Move assignment Operator\n";
	}
	
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
	bool m_reset;
	bool m_awaitingRegeneration;
	VertexBuffer m_vertexBuffer;
	unsigned int m_ID;
	unsigned int m_transparentID;
	int m_opaqueElementBufferIndex;
	int m_transparentElementBufferIndex;

	VertexArray* m_next;
};