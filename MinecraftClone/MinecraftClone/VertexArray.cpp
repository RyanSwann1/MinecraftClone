#include "VertexArray.h"
#include "glad.h"
#include "VertexBuffer.h"
#include "Utilities.h"
#include <iostream>

VertexArray::VertexArray()
{
	glGenVertexArrays(1, &m_ID);
	glGenBuffers(1, &m_vertexBuffer.positionsID);
	glGenBuffers(1, &m_vertexBuffer.textCoordsID);
	glGenBuffers(1, &m_vertexBuffer.indiciesID);

	std::cout << m_ID << "\n";
}

VertexArray::~VertexArray()
{
	std::cout << "Destroyed VAO\n";
	glDeleteVertexArrays(1, &m_ID);

	glDeleteBuffers(1, &m_vertexBuffer.positionsID);
	glDeleteBuffers(1, &m_vertexBuffer.textCoordsID);
	glDeleteBuffers(1, &m_vertexBuffer.indiciesID);
}

void VertexArray::reset()
{
	m_vertexBuffer.positions.clear();
	m_vertexBuffer.textCoords.clear();
	m_vertexBuffer.indicies.clear();
}

void VertexArray::init() const
{
	bind();

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer.positionsID);
	glBufferData(GL_ARRAY_BUFFER, m_vertexBuffer.positions.size() * sizeof(glm::vec3), m_vertexBuffer.positions.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (const void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer.textCoordsID);
	glBufferData(GL_ARRAY_BUFFER, m_vertexBuffer.textCoords.size() * sizeof(glm::vec2), m_vertexBuffer.textCoords.data(), GL_STATIC_DRAW);
		
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (const void*)(0));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vertexBuffer.indiciesID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_vertexBuffer.indicies.size() * sizeof(unsigned int), m_vertexBuffer.indicies.data(), GL_STATIC_DRAW);

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