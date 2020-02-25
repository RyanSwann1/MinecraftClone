#pragma once

#include "VertexBuffer.h"
#include "glm/glm.hpp"

struct VertexArray
{
	VertexArray();
	~VertexArray();
	VertexArray(const VertexArray&) = delete;
	VertexArray& operator=(const VertexArray&) = delete;
	VertexArray(VertexArray&&) noexcept;
	VertexArray& operator=(VertexArray&&) noexcept;
	
	glm::vec3 getOwningChunkStartingPosition() const;

	void init(VertexBuffer& vertexBuffer);
	void bind() const;
	void unbind() const;

	VertexBuffer m_vertexBuffer;
	unsigned int m_ID;
	glm::vec3 m_owningChunkStartingPosition;
};