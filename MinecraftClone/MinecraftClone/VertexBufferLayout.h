#pragma once

#include "glad.h"
#include <vector>

struct VertexBufferElement
{
	unsigned int type;
	unsigned int count;
	unsigned char normalized;

	static unsigned int getSizeOfType(unsigned int type)
	{
		switch (type)
		{
		case GL_FLOAT:
			return sizeof(float);
		case GL_UNSIGNED_INT :
			return sizeof(int);
		case GL_UNSIGNED_BYTE :
			return 1;

		}

		return 0;
	}
};

class VertexBufferLayout
{
public:
	VertexBufferLayout();

	template <typename T>
	void push(unsigned int count)
	{
		static_assert(false);
	}

	template <>
	void push<float>(unsigned int count)
	{
		m_elements.push_back({ GL_FLOAT, count, GL_FALSE });
		m_stride += count * VertexBufferElement::getSizeOfType(GL_FLOAT);
	}

	template <>
	void push<unsigned int>(unsigned int count)
	{
		m_elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
		m_stride += count * VertexBufferElement::getSizeOfType(GL_UNSIGNED_INT);
	}

	template <>
	void push<unsigned char>(unsigned int count)
	{
		m_elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
		m_stride += count * VertexBufferElement::getSizeOfType(GL_UNSIGNED_BYTE);
	}

	unsigned int getStride() const; 
	const std::vector<VertexBufferElement>& getElements() const;

private:
	std::vector<VertexBufferElement> m_elements;
	unsigned int m_stride;
};