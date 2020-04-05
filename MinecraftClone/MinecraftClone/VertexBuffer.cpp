#include "VertexBuffer.h"
#include "VertexArray.h"
#include <iostream>

VertexBuffer::VertexBuffer()
	: elementBufferIndex(0),
	bindToVAO(false),
	displayable(false),
	positionsID(Utilities::INVALID_OPENGL_ID),
	positions(),
	lightIntensityID(Utilities::INVALID_OPENGL_ID),
	lightIntensityVertices(),
	textCoordsID(Utilities::INVALID_OPENGL_ID),
	textCoords(),
	indiciesID(Utilities::INVALID_OPENGL_ID),
	indicies()
{
	glGenBuffers(1, &positionsID);
	glGenBuffers(1, &lightIntensityID);
	glGenBuffers(1, &textCoordsID);
	glGenBuffers(1, &indiciesID);
}

VertexBuffer::VertexBuffer(VertexBuffer&& orig) noexcept
	: elementBufferIndex(orig.elementBufferIndex),
	bindToVAO(orig.bindToVAO),
	displayable(orig.displayable),
	positionsID(orig.positionsID),
	positions(std::move(orig.positions)),
	lightIntensityID(orig.lightIntensityID),
	lightIntensityVertices(std::move(orig.lightIntensityVertices)),
	textCoordsID(orig.textCoordsID),
	textCoords(std::move(orig.textCoords)),
	indiciesID(orig.indiciesID),
	indicies(std::move(orig.indicies))
{
	orig.elementBufferIndex = 0;
	orig.bindToVAO = false;
	orig.displayable = false;
	orig.positionsID = Utilities::INVALID_OPENGL_ID;
	orig.textCoordsID = Utilities::INVALID_OPENGL_ID;
	orig.indiciesID = Utilities::INVALID_OPENGL_ID;
	orig.lightIntensityID = Utilities::INVALID_OPENGL_ID;
}

VertexBuffer& VertexBuffer::operator=(VertexBuffer&& orig) noexcept
{
	elementBufferIndex = orig.elementBufferIndex;
	bindToVAO = orig.bindToVAO;
	displayable = orig.displayable;
	positionsID = orig.positionsID;
	positions = std::move(orig.positions);
	lightIntensityID = orig.lightIntensityID;
	lightIntensityVertices = std::move(orig.lightIntensityVertices);
	textCoordsID = orig.textCoordsID;
	textCoords = std::move(orig.textCoords);
	indiciesID = orig.indiciesID;
	indicies = std::move(orig.indicies);

	orig.elementBufferIndex = 0;
	orig.bindToVAO = false;
	orig.displayable = false;
	orig.positionsID = Utilities::INVALID_OPENGL_ID;
	orig.textCoordsID = Utilities::INVALID_OPENGL_ID;
	orig.indiciesID = Utilities::INVALID_OPENGL_ID;
	orig.lightIntensityID = Utilities::INVALID_OPENGL_ID;

	return *this;
}

VertexBuffer::~VertexBuffer()
{
	assert(positionsID != Utilities::INVALID_OPENGL_ID);
	glDeleteBuffers(1, &positionsID);
	
	assert(lightIntensityID != Utilities::INVALID_OPENGL_ID);
	glDeleteBuffers(1, &lightIntensityID);

	assert(textCoordsID != Utilities::INVALID_OPENGL_ID);
	glDeleteBuffers(1, &textCoordsID);
	
	assert(indiciesID != Utilities::INVALID_OPENGL_ID);
	glDeleteBuffers(1, &indiciesID);
}

void VertexBuffer::bind()
{
	assert(bindToVAO);
	bindToVAO = false;
	displayable = true;

	glBindBuffer(GL_ARRAY_BUFFER, positionsID);
	glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::ivec3), positions.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_INT, GL_FALSE, sizeof(glm::ivec3), (const void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, textCoordsID);
	glBufferData(GL_ARRAY_BUFFER, textCoords.size() * sizeof(glm::vec3), textCoords.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (const void*)(0));

	glBindBuffer(GL_ARRAY_BUFFER, lightIntensityID);
	glBufferData(GL_ARRAY_BUFFER, lightIntensityVertices.size() * sizeof(float), lightIntensityVertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(float), (const void*)(0));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indiciesID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies.size() * sizeof(unsigned int), indicies.data(), GL_STATIC_DRAW);

	unbind();

	std::vector<glm::ivec3> newPositions;
	positions.swap(newPositions);

	std::vector<glm::vec3> newTextCoords;
	textCoords.swap(newTextCoords);

	std::vector<float> newLightIntensityVertices;
	lightIntensityVertices.swap(newLightIntensityVertices);

	indicies.shrink_to_fit();
}

void VertexBuffer::unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::clear()
{
	elementBufferIndex = 0;
	bindToVAO = false;
	displayable = false;

	std::vector<glm::ivec3> newPositions;
	positions.swap(newPositions);

	std::vector<float> newLightIntensityVertices;
	lightIntensityVertices.swap(newLightIntensityVertices);

	std::vector<glm::vec3> newTextCoords;
	textCoords.swap(newTextCoords);
	
	std::vector<unsigned int> newIndicies;
	indicies.swap(newIndicies);
}