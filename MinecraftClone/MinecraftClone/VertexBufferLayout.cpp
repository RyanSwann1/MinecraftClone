#include "VertexBufferLayout.h"

VertexBufferLayout::VertexBufferLayout()
	: m_elements(),
	m_stride(0)
{
}

unsigned int VertexBufferLayout::getStride() const
{
	return 0;
}

const std::vector<VertexBufferElement>& VertexBufferLayout::getElements() const
{
	return m_elements;
}
