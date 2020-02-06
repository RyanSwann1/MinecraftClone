#include "IndexBuffer.h"
#include "glad.h"
#include <assert.h>

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count)
	: m_rendererID(0),
	m_count(count)
{
	assert(sizeof(unsigned int) == sizeof(GLuint));

	glGenBuffers(1, &m_rendererID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_count * sizeof(unsigned int), data, GL_STATIC_DRAW);
}

IndexBuffer::~IndexBuffer()
{
	glDeleteBuffers(1, &m_rendererID);
}

unsigned int IndexBuffer::getCount() const
{
	return m_count;
}

void IndexBuffer::bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);
}

void IndexBuffer::unbind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
