#pragma once

#include "glm/glm.hpp"

struct VertexBuffer;
class VertexArray
{
public:
	VertexArray();
	~VertexArray();
	
	glm::vec3 getOwningChunkStartingPosition() const;

	void init(VertexBuffer& vertexBuffer);
	void bind() const;
	void unbind() const;

private:
	unsigned int m_ID;
	glm::vec3 m_owningChunkStartingPosition;
};