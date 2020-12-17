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
	onDestroy();
}

VertexArray::VertexArray(VertexArray&& rhs) noexcept
	: m_opaqueVertexBuffer(std::move(rhs.m_opaqueVertexBuffer)),
	m_transparentVertexBuffer(std::move(rhs.m_transparentVertexBuffer)),
	m_opaqueID(rhs.m_opaqueID),
	m_transparentID(rhs.m_transparentID)
{
	rhs.m_opaqueID = Globals::INVALID_OPENGL_ID; 
	rhs.m_transparentID = Globals::INVALID_OPENGL_ID;
}

VertexArray& VertexArray::operator=(VertexArray&& rhs) noexcept
{
	assert(this != &rhs);
	if (this != &rhs)
	{
		onDestroy();

		m_opaqueVertexBuffer = std::move(rhs.m_opaqueVertexBuffer);
		m_transparentVertexBuffer = std::move(rhs.m_transparentVertexBuffer);

		m_opaqueID = rhs.m_opaqueID;
		m_transparentID = rhs.m_transparentID;

		rhs.m_opaqueID = Globals::INVALID_OPENGL_ID;
		rhs.m_transparentID = Globals::INVALID_OPENGL_ID;
	}

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
}

void VertexArray::attachTransparentVBO()
{	
	bindTransparentVAO();
	m_transparentVertexBuffer.bind();
}

void VertexArray::bindOpaqueVAO() const
{
	glBindVertexArray(m_opaqueID);
}

void VertexArray::bindTransparentVAO() const
{
	glBindVertexArray(m_transparentID);
}

void VertexArray::onDestroy()
{
	if (m_opaqueID != Globals::INVALID_OPENGL_ID &&
		m_transparentID != Globals::INVALID_OPENGL_ID)
	{
		glDeleteVertexArrays(1, &m_opaqueID);
		glDeleteVertexArrays(1, &m_transparentID);
	}
	else
	{
		assert(m_opaqueID == Globals::INVALID_OPENGL_ID && m_transparentID == Globals::INVALID_OPENGL_ID);
	}
}
