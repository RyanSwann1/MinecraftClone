#include "VertexArray.h"
#include "glad.h"
#include "VertexBuffer.h"
#include "Utilities.h"
#include <iostream>

VertexArray::VertexArray()
	: m_active(true),
	m_readyToBind(false)
{
	glGenVertexArrays(1, &m_ID);
}

VertexArray::~VertexArray()
{
	if (m_ID != Utilities::INVALID_OPENGL_ID)
	{
		glDeleteVertexArrays(1, &m_ID);
	}
}

VertexArray::VertexArray(VertexArray&& orig) noexcept
	: m_ID(orig.m_ID),
	m_owningChunkStartingPosition(orig.m_owningChunkStartingPosition),
	m_active(true)
{
	orig.m_ID = Utilities::INVALID_OPENGL_ID;
	orig.m_active = false;
}

VertexArray& VertexArray::operator=(VertexArray&& orig) noexcept
{
	this->m_ID = orig.m_ID;
	orig.m_ID = Utilities::INVALID_OPENGL_ID;
	this->m_owningChunkStartingPosition = orig.m_owningChunkStartingPosition;
	orig.deactivate();

	return *this;
}

glm::vec3 VertexArray::getOwningChunkStartingPosition() const
{
	return m_owningChunkStartingPosition;
}

void VertexArray::deactivate()
{
	m_active = false;
}

void VertexArray::init(VertexBuffer& vertexBuffer)
{
	m_owningChunkStartingPosition = vertexBuffer.m_owningChunkStartingPosition;
	m_readyToBind = true;
	bind();
	glGenBuffers(1, &vertexBuffer.positionsID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer.positionsID);
	glBufferData(GL_ARRAY_BUFFER, vertexBuffer.positions.size() * sizeof(glm::vec3), vertexBuffer.positions.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (const void*)0);

	glGenBuffers(1, &vertexBuffer.textCoordsID);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer.textCoordsID);
	glBufferData(GL_ARRAY_BUFFER, vertexBuffer.textCoords.size() * sizeof(glm::vec2), vertexBuffer.textCoords.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (const void*)(0));

	glGenBuffers(1, &vertexBuffer.indiciesID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexBuffer.indiciesID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexBuffer.indicies.size() * sizeof(unsigned int), vertexBuffer.indicies.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	unbind();
}

void VertexArray::bind() const
{
	glBindVertexArray(m_ID);
}

void VertexArray::unbind() const
{
	glBindVertexArray(0);
}
