#include "VertexArray.h"
#include "glad.h"
#include "VertexBuffer.h"
#include "Utilities.h"
#include <iostream>

VertexArray::VertexArray()
	: m_inUse(false),
	m_awaitingRegeneration(false),
	m_opaqueVertexBuffer(),
	m_transparentVertexBuffer(),
	m_opaqueID(Utilities::INVALID_OPENGL_ID),
	m_transparentID(Utilities::INVALID_OPENGL_ID)
{
	glGenVertexArrays(1, &m_opaqueID);
	glGenVertexArrays(1, &m_transparentID);
}

VertexArray::~VertexArray()
{
	assert(m_opaqueID != Utilities::INVALID_OPENGL_ID);
	glDeleteVertexArrays(1, &m_opaqueID);
	
	assert(m_transparentID != Utilities::INVALID_OPENGL_ID);
	glDeleteVertexArrays(1, &m_transparentID);
}

VertexArray::VertexArray(VertexArray&& orig) noexcept
	: m_inUse(orig.m_inUse),
	m_awaitingRegeneration(orig.m_awaitingRegeneration),
	m_opaqueVertexBuffer(std::move(orig.m_opaqueVertexBuffer)),
	m_transparentVertexBuffer(std::move(orig.m_transparentVertexBuffer)),
	m_opaqueID(orig.m_opaqueID),
	m_transparentID(orig.m_transparentID)
{
	orig.m_inUse = false;
	orig.m_awaitingRegeneration = false;
	orig.m_opaqueID = Utilities::INVALID_OPENGL_ID; 
	orig.m_transparentID = Utilities::INVALID_OPENGL_ID;
}

VertexArray& VertexArray::operator=(VertexArray&& orig) noexcept
{
	m_inUse = orig.m_inUse;
	m_awaitingRegeneration = orig.m_awaitingRegeneration;
	m_opaqueVertexBuffer = std::move(orig.m_opaqueVertexBuffer);
	m_transparentVertexBuffer = std::move(orig.m_transparentVertexBuffer);
	m_opaqueID = orig.m_opaqueID;
	m_transparentID = orig.m_transparentID;
	
	orig.m_inUse = false;
	orig.m_awaitingRegeneration = false;
	orig.m_opaqueID = Utilities::INVALID_OPENGL_ID;
	orig.m_transparentID = Utilities::INVALID_OPENGL_ID;

	return *this;
}

bool VertexArray::isInUse() const
{
	return m_inUse;
}

void VertexArray::reset()
{
	m_inUse = false;
	m_awaitingRegeneration = false;
	m_opaqueVertexBuffer.clear();
	m_transparentVertexBuffer.clear();
}

void VertexArray::attachOpaqueVBO()
{
	bindOpaqueVAO();
	m_opaqueVertexBuffer.bind();
	unbind();
}

void VertexArray::attachTransparentVBO()
{	
	bindTransparentVAO();
	m_transparentVertexBuffer.bind();
	unbind();
}

void VertexArray::bindOpaqueVAO() const
{
	glBindVertexArray(m_opaqueID);
}

void VertexArray::bindTransparentVAO() const
{
	glBindVertexArray(m_transparentID);
}

void VertexArray::unbind() const
{
	glBindVertexArray(0);
}