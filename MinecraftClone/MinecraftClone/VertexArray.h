#pragma once

#include "glm/glm.hpp"

struct VertexBuffer;
class VertexArray
{
public:
	VertexArray();
	~VertexArray();
	VertexArray(const VertexArray&) = delete;
	VertexArray& operator=(const VertexArray&) = delete;
	VertexArray(VertexArray&&) noexcept;
	VertexArray& operator=(VertexArray&&) noexcept;
	
	bool isActive() const;
	bool isReadyToAttachToVBO() const;
	void setOwningStartingPosition(glm::vec3 owningStartingPosition);
	glm::vec3 getOwningChunkStartingPosition() const;
	
	void deactivate();
	void attachToVBO(VertexBuffer& vertexBuffer);
	void bind() const;
	void unbind() const;

private:
	unsigned int m_ID;
	glm::vec3 m_owningChunkStartingPosition;
	bool m_active;
	bool m_attachToVBO;
};