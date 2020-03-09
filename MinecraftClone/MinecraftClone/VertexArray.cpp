#include "VertexArray.h"
#include "glad.h"
#include "VertexBuffer.h"
#include "Utilities.h"
#include <iostream>

VertexArray::VertexArray()
	: m_inUse(false),
	m_opaqueVBODisplayable(false),
	m_transparentVBODisplayable(false),
	m_attachOpaqueVBO(false),
	m_attachTransparentVBO(false),
	m_reset(false),
	m_awaitingRegeneration(false),
	m_vertexBuffer(),
	m_ID(Utilities::INVALID_OPENGL_ID),
	m_transparentID(Utilities::INVALID_OPENGL_ID),
	m_opaqueElementBufferIndex(0),
	m_transparentElementBufferIndex(0),
	m_next(nullptr)
{}

void VertexArray::reset()
{
	if (m_ID != Utilities::INVALID_OPENGL_ID)
	{
		glDeleteVertexArrays(1, &m_ID);
	}
	
	if (m_transparentID != Utilities::INVALID_OPENGL_ID)
	{
		glDeleteVertexArrays(1, &m_transparentID);
	}

	if (m_vertexBuffer.opaqueVerticesID != Utilities::INVALID_OPENGL_ID)
	{
		glDeleteBuffers(1, &m_vertexBuffer.opaqueVerticesID);
	}

	if (m_vertexBuffer.transparentVerticesID != Utilities::INVALID_OPENGL_ID)
	{
		glDeleteBuffers(1, &m_vertexBuffer.transparentVerticesID);
	}

	if (m_vertexBuffer.indiciesID != Utilities::INVALID_OPENGL_ID)
	{
		glDeleteBuffers(1, &m_vertexBuffer.indiciesID);
	}

	if (m_vertexBuffer.transparentIndiciesID != Utilities::INVALID_OPENGL_ID) 
	{
		glDeleteBuffers(1, &m_vertexBuffer.transparentIndiciesID);
	}

	m_reset = false;
	m_inUse = false;
	m_opaqueVBODisplayable = false;
	m_transparentVBODisplayable = false;
	m_attachOpaqueVBO = false;
	m_attachTransparentVBO = false;
	m_awaitingRegeneration = false;
	m_vertexBuffer.clear();
	m_ID = Utilities::INVALID_OPENGL_ID;
	m_transparentID = Utilities::INVALID_OPENGL_ID;
	m_opaqueElementBufferIndex = 0;
	m_transparentElementBufferIndex = 0;
}

void VertexArray::attachOpaqueVBO()
{
	m_attachOpaqueVBO = false;

	if (m_ID == Utilities::INVALID_OPENGL_ID) 
	{
		glGenVertexArrays(1, &m_ID);
	}

	bindOpaqueVAO();

	if (m_vertexBuffer.opaqueVerticesID == Utilities::INVALID_OPENGL_ID)
	{
		glGenBuffers(1, &m_vertexBuffer.opaqueVerticesID);
	}
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer.opaqueVerticesID);
	glBufferData(GL_ARRAY_BUFFER, m_vertexBuffer.opaqueVertices.size() * sizeof(float), m_vertexBuffer.opaqueVertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (const void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (const void*)(3 * sizeof(float)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	/*if (m_vertexBuffer.positionsID == Utilities::INVALID_OPENGL_ID)
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
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (const void*)(0));*/

	if (m_vertexBuffer.indiciesID == Utilities::INVALID_OPENGL_ID)
	{
		glGenBuffers(1, &m_vertexBuffer.indiciesID);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vertexBuffer.indiciesID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_vertexBuffer.indicies.size() * sizeof(unsigned int), m_vertexBuffer.indicies.data(), GL_STATIC_DRAW);

	unbind();
	if (!m_awaitingRegeneration)
	{
		m_opaqueVBODisplayable = true;
	}
}

void VertexArray::attachTransparentVBO()
{
	if (m_transparentID == Utilities::INVALID_OPENGL_ID)
	{
		glGenVertexArrays(1, &m_transparentID);
	}

	m_attachTransparentVBO = false;
	
	bindTransparentVAO();

	if (m_vertexBuffer.transparentPositionsID == Utilities::INVALID_OPENGL_ID)
	{
		glGenBuffers(1, &m_vertexBuffer.transparentPositionsID);
	}
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer.transparentPositionsID);
	glBufferData(GL_ARRAY_BUFFER, m_vertexBuffer.transparentPositions.size() * sizeof(glm::vec3), m_vertexBuffer.transparentPositions.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (const void*)0);

	if (m_vertexBuffer.transparentTextCoordsID == Utilities::INVALID_OPENGL_ID)
	{
		glGenBuffers(1, &m_vertexBuffer.transparentTextCoordsID);
	}
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer.transparentTextCoordsID);
	glBufferData(GL_ARRAY_BUFFER, m_vertexBuffer.transparentTextCoords.size() * sizeof(glm::vec2), m_vertexBuffer.transparentTextCoords.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (const void*)(0));

	if (m_vertexBuffer.transparentIndiciesID == Utilities::INVALID_OPENGL_ID)
	{
		glGenBuffers(1, &m_vertexBuffer.transparentIndiciesID);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vertexBuffer.transparentIndiciesID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_vertexBuffer.transparentIndicies.size() * sizeof(unsigned int), m_vertexBuffer.transparentIndicies.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	
	unbind();

	if (!m_awaitingRegeneration)
	{
		m_transparentVBODisplayable = true;
	}
}

void VertexArray::bindOpaqueVAO() const
{
	glBindVertexArray(m_ID);
}

void VertexArray::bindTransparentVAO() const
{
	glBindVertexArray(m_transparentID);
}

void VertexArray::unbind() const
{
	glBindVertexArray(0);
}