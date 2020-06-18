#include "VertexArray.h"
#include "glad.h"
#include "VertexBuffer.h"
#include "Globals.h"
#include <iostream>

VertexArray::VertexArray()
	: m_opaqueVertexBuffer(),
	m_transparentVertexBuffer(),
	m_opaqueID(Globals::INVALID_OPENGL_ID),
	m_transparentID(Globals::INVALID_OPENGL_ID)
{
	glGenVertexArrays(1, &m_opaqueID);
	glGenVertexArrays(1, &m_transparentID);
}

VertexArray::~VertexArray()
{
	assert(m_opaqueID != Globals::INVALID_OPENGL_ID);
	glDeleteVertexArrays(1, &m_opaqueID);
	
	assert(m_transparentID != Globals::INVALID_OPENGL_ID);
	glDeleteVertexArrays(1, &m_transparentID);
}

VertexArray::VertexArray(VertexArray&& orig) noexcept
	: m_opaqueVertexBuffer(std::move(orig.m_opaqueVertexBuffer)),
	m_transparentVertexBuffer(std::move(orig.m_transparentVertexBuffer)),
	m_opaqueID(orig.m_opaqueID),
	m_transparentID(orig.m_transparentID)
{
	orig.m_opaqueID = Globals::INVALID_OPENGL_ID; 
	orig.m_transparentID = Globals::INVALID_OPENGL_ID;
}

VertexArray& VertexArray::operator=(VertexArray&& orig) noexcept
{
	m_opaqueVertexBuffer = std::move(orig.m_opaqueVertexBuffer);
	m_transparentVertexBuffer = std::move(orig.m_transparentVertexBuffer);
	m_opaqueID = orig.m_opaqueID;
	m_transparentID = orig.m_transparentID;

	orig.m_opaqueID = Globals::INVALID_OPENGL_ID;
	orig.m_transparentID = Globals::INVALID_OPENGL_ID;

	return *this;
}

void VertexArray::reset()
{
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