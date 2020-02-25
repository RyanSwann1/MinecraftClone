#pragma once

#include "VertexBuffer.h"
#include "glm/glm.hpp"

struct VertexArray
{
	VertexArray();
	~VertexArray();
	VertexArray(const VertexArray&) = delete;
	VertexArray& operator=(const VertexArray&) = delete;
	VertexArray(VertexArray&&) = delete; // noexcept;
	VertexArray& operator=(VertexArray&&) = delete; // noexcept;
	
	void init() const;
	void bind() const;
	void unbind() const;

	VertexBuffer m_vertexBuffer;
	unsigned int m_ID;
};