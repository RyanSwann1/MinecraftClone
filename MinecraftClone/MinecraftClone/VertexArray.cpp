#include "VertexArray.h"
#include "glad.h"
#include "VertexBuffer.h"
#include "Utilities.h"
#include <iostream>

VertexArray::VertexArray()
	: m_displayable(false),
	m_attachOpaqueVBO(false),
	m_attachTransparentVBO(false),
	m_destroy(false),
	m_vertexBuffer(),
	m_ID(Utilities::INVALID_OPENGL_ID)
{}

void VertexArray::reset()
{
	m_vertexBuffer.positions.clear();
	m_vertexBuffer.textCoords.clear();
	m_vertexBuffer.indicies.clear();

	m_vertexBuffer.transparentPositions.clear();
	m_vertexBuffer.transparentTextCoords.clear();
	m_vertexBuffer.transparentIndicies.clear();
}

void VertexArray::destroy()
{
	m_destroy = false;
	
	if (m_ID != Utilities::INVALID_OPENGL_ID)
	{
		glDeleteVertexArrays(1, &m_ID);
	}
	if (m_vertexBuffer.positionsID != Utilities::INVALID_OPENGL_ID)
	{
		glDeleteBuffers(1, &m_vertexBuffer.positionsID);
	}
	if (m_vertexBuffer.textCoordsID != Utilities::INVALID_OPENGL_ID)
	{
		glDeleteBuffers(1, &m_vertexBuffer.textCoordsID);
	}
	if (m_vertexBuffer.indiciesID != Utilities::INVALID_OPENGL_ID)
	{
		glDeleteBuffers(1, &m_vertexBuffer.indiciesID);
	}
}

void VertexArray::attachOpaqueVBO()
{
	m_attachOpaqueVBO = false;
	
	if (m_ID == Utilities::INVALID_OPENGL_ID) 
	{
		glGenVertexArrays(1, &m_ID);
	}
	bind();

	if (m_vertexBuffer.positionsID == Utilities::INVALID_OPENGL_ID)
	{
		glGenBuffers(1, &m_vertexBuffer.positionsID);
	}
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer.positionsID);
	glBufferData(GL_ARRAY_BUFFER, m_vertexBuffer.positions.size() * sizeof(glm::vec3), m_vertexBuffer.positions.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (const void*)0);

	if (m_vertexBuffer.textCoordsID == Utilities::INVALID_OPENGL_ID)
	{
		glGenBuffers(1, &m_vertexBuffer.textCoordsID);
	}
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer.textCoordsID);
	glBufferData(GL_ARRAY_BUFFER, m_vertexBuffer.textCoords.size() * sizeof(glm::vec2), m_vertexBuffer.textCoords.data(), GL_STATIC_DRAW);
		
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (const void*)(0));

	if (m_vertexBuffer.indiciesID == Utilities::INVALID_OPENGL_ID)
	{
		glGenBuffers(1, &m_vertexBuffer.indiciesID);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vertexBuffer.indiciesID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_vertexBuffer.indicies.size() * sizeof(unsigned int), m_vertexBuffer.indicies.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	unbind();
}

void VertexArray::attachTransparentVBO()
{
	assert(m_ID != Utilities::INVALID_OPENGL_ID);
	if (m_ID == Utilities::INVALID_OPENGL_ID)
	{
		return;
	}

	m_attachTransparentVBO = false;
	
	bind();

	if (m_vertexBuffer.transparentPositionsID == Utilities::INVALID_OPENGL_ID)
	{
		glGenBuffers(1, &m_vertexBuffer.transparentPositionsID);
	}
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer.transparentPositionsID);
	glBufferData(GL_ARRAY_BUFFER, m_vertexBuffer.transparentPositions.size() * sizeof(glm::vec3), m_vertexBuffer.transparentPositions.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (const void*)0);

	if (m_vertexBuffer.textCoordsID == Utilities::INVALID_OPENGL_ID)
	{
		glGenBuffers(1, &m_vertexBuffer.transparentTextCoordsID);
	}
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer.transparentTextCoordsID);
	glBufferData(GL_ARRAY_BUFFER, m_vertexBuffer.transparentTextCoords.size() * sizeof(glm::vec2), m_vertexBuffer.transparentTextCoords.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (const void*)(0));

	if (m_vertexBuffer.indiciesID == Utilities::INVALID_OPENGL_ID)
	{
		glGenBuffers(1, &m_vertexBuffer.transparentIndiciesID);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vertexBuffer.transparentIndiciesID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_vertexBuffer.transparentIndicies.size() * sizeof(unsigned int), m_vertexBuffer.transparentIndicies.data(), GL_STATIC_DRAW);

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