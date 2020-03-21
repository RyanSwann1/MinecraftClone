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
	m_transparentElementBufferIndex(0)
{
	glGenVertexArrays(1, &m_ID);
	glGenVertexArrays(1, &m_transparentID);
}

VertexArray::~VertexArray()
{
	assert(m_ID != Utilities::INVALID_OPENGL_ID);
	glDeleteVertexArrays(1, &m_ID);
	
	assert(m_transparentID != Utilities::INVALID_OPENGL_ID);
	glDeleteVertexArrays(1, &m_transparentID);
}

VertexArray::VertexArray(VertexArray&& orig) noexcept
	: m_inUse(orig.m_inUse),
	m_opaqueVBODisplayable(orig.m_opaqueVBODisplayable),
	m_transparentVBODisplayable(orig.m_transparentVBODisplayable),
	m_attachOpaqueVBO(orig.m_attachOpaqueVBO),
	m_attachTransparentVBO(orig.m_attachTransparentVBO),
	m_reset(orig.m_reset),
	m_awaitingRegeneration(orig.m_awaitingRegeneration),
	m_vertexBuffer(std::move(orig.m_vertexBuffer)),
	m_ID(orig.m_ID),
	m_transparentID(orig.m_transparentID),
	m_opaqueElementBufferIndex(orig.m_opaqueElementBufferIndex),
	m_transparentElementBufferIndex(orig.m_transparentElementBufferIndex)
{
	orig.m_inUse = false;
	orig.m_opaqueVBODisplayable = false;
	orig.m_transparentVBODisplayable = false;
	orig.m_attachOpaqueVBO = false;
	orig.m_attachTransparentVBO = false;
	orig.m_reset = false;
	orig.m_awaitingRegeneration = false;
	orig.m_ID = Utilities::INVALID_OPENGL_ID;
	orig.m_transparentID = Utilities::INVALID_OPENGL_ID;
	orig.m_opaqueElementBufferIndex = 0;
	orig.m_transparentElementBufferIndex = 0;
}

VertexArray& VertexArray::operator=(VertexArray&& orig) noexcept
{
	m_inUse = orig.m_inUse;
	m_opaqueVBODisplayable = orig.m_opaqueVBODisplayable;
	m_transparentVBODisplayable = orig.m_transparentVBODisplayable;
	m_attachOpaqueVBO = orig.m_attachOpaqueVBO;
	m_attachTransparentVBO = orig.m_attachTransparentVBO;
	m_reset = orig.m_reset;
	m_awaitingRegeneration = orig.m_awaitingRegeneration;
	m_vertexBuffer = std::move(orig.m_vertexBuffer);
	m_ID = orig.m_ID;
	m_transparentID = orig.m_transparentID;
	m_opaqueElementBufferIndex = orig.m_opaqueElementBufferIndex;
	m_transparentElementBufferIndex = orig.m_transparentElementBufferIndex;
	
	orig.m_inUse = false;
	orig.m_opaqueVBODisplayable = false;
	orig.m_transparentVBODisplayable = false;
	orig.m_attachOpaqueVBO = false;
	orig.m_attachTransparentVBO = false;
	orig.m_reset = false;
	orig.m_awaitingRegeneration = false;
	orig.m_ID = Utilities::INVALID_OPENGL_ID;
	orig.m_transparentID = Utilities::INVALID_OPENGL_ID;
	orig.m_opaqueElementBufferIndex = 0;
	orig.m_transparentElementBufferIndex = 0;

	return *this;
}

bool VertexArray::isInUse() const
{
	return m_inUse;
}

void VertexArray::reset()
{
	m_reset = false;
	m_inUse = false;
	m_opaqueVBODisplayable = false;
	m_transparentVBODisplayable = false;
	m_attachOpaqueVBO = false;
	m_attachTransparentVBO = false;
	m_awaitingRegeneration = false;
	m_vertexBuffer.clear();
	m_opaqueElementBufferIndex = 0;
	m_transparentElementBufferIndex = 0;
}

void VertexArray::attachOpaqueVBO()
{
	assert(m_attachOpaqueVBO);
	m_attachOpaqueVBO = false;

	bindOpaqueVAO();

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer.positionsID);
	glBufferData(GL_ARRAY_BUFFER, m_vertexBuffer.positions.size() * sizeof(glm::ivec3), m_vertexBuffer.positions.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_INT, GL_FALSE, 3 * sizeof(int), (const void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer.textCoordsID);
	glBufferData(GL_ARRAY_BUFFER, m_vertexBuffer.textCoords.size() * sizeof(glm::vec2), m_vertexBuffer.textCoords.data(), GL_STATIC_DRAW);
		
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (const void*)(0));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vertexBuffer.indiciesID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_vertexBuffer.indicies.size() * sizeof(unsigned int), m_vertexBuffer.indicies.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	unbind();

	assert(!m_opaqueVBODisplayable);
	m_opaqueVBODisplayable = true;
}

void VertexArray::attachTransparentVBO()
{
	assert(m_attachTransparentVBO);
	m_attachTransparentVBO = false;
	
	bindTransparentVAO();

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer.transparentPositionsID);
	glBufferData(GL_ARRAY_BUFFER, m_vertexBuffer.transparentPositions.size() * sizeof(glm::ivec3), m_vertexBuffer.transparentPositions.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_INT, GL_FALSE, 3 * sizeof(int), (const void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer.transparentTextCoordsID);
	glBufferData(GL_ARRAY_BUFFER, m_vertexBuffer.transparentTextCoords.size() * sizeof(glm::vec2), m_vertexBuffer.transparentTextCoords.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (const void*)(0));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vertexBuffer.transparentIndiciesID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_vertexBuffer.transparentIndicies.size() * sizeof(unsigned int), m_vertexBuffer.transparentIndicies.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	unbind();

	assert(!m_transparentVBODisplayable);
	m_transparentVBODisplayable = true;
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