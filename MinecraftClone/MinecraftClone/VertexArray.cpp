#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "VertexBuffer.h"

VertexArray::VertexArray()
{
	glGenVertexArrays(1, &m_rendererID);
	glBindVertexArray(m_rendererID);
}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &m_rendererID);
}

void VertexArray::bind() const
{
	glBindVertexArray(m_rendererID);
}

void VertexArray::unbind() const
{
	glBindVertexArray(0);
}

void VertexArray::addBuffer(const VertexBuffer& vertexBuffer, const VertexBufferLayout layout)
{
	bind();
	vertexBuffer.bind();

	const std::vector<VertexBufferElement>& elements = layout.getElements();
	unsigned int offset = 0;
	for (unsigned int i = 0; i < elements.size(); ++i)
	{
		const VertexBufferElement& element = elements[i];
		glEnableVertexAttribArray(i);
		glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.getStride(), (const void*)offset);
		offset += element.count * VertexBufferElement::getSizeOfType(element.type);
	}
}