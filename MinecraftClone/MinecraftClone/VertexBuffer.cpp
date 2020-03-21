#include "VertexBuffer.h"
#include <iostream>

VertexBuffer::VertexBuffer()
	: positionsID(Utilities::INVALID_OPENGL_ID),
	positions(),
	textCoordsID(Utilities::INVALID_OPENGL_ID),
	textCoords(),
	indiciesID(Utilities::INVALID_OPENGL_ID),
	indicies(),
	transparentPositionsID(Utilities::INVALID_OPENGL_ID),
	transparentPositions(),
	transparentTextCoordsID(Utilities::INVALID_OPENGL_ID),
	transparentTextCoords(),
	transparentIndiciesID(Utilities::INVALID_OPENGL_ID),
	transparentIndicies()
{
	glGenBuffers(1, &positionsID);
	glGenBuffers(1, &textCoordsID);
	glGenBuffers(1, &indiciesID);

	glGenBuffers(1, &transparentPositionsID);
	glGenBuffers(1, &transparentTextCoordsID);
	glGenBuffers(1, &transparentIndiciesID);
}

VertexBuffer::VertexBuffer(VertexBuffer&& orig) noexcept
	: positionsID(orig.positionsID),
	positions(std::move(orig.positions)),
	textCoordsID(orig.textCoordsID),
	textCoords(std::move(orig.textCoords)),
	indiciesID(orig.indiciesID),
	indicies(std::move(orig.indicies)),
	transparentPositionsID(orig.transparentPositionsID),
	transparentPositions(std::move(orig.transparentPositions)),
	transparentTextCoordsID(orig.transparentTextCoordsID),
	transparentTextCoords(std::move(orig.transparentTextCoords)),
	transparentIndiciesID(orig.transparentIndiciesID),
	transparentIndicies(std::move(orig.transparentIndicies))
{
	orig.positionsID = Utilities::INVALID_OPENGL_ID;
	orig.textCoordsID = Utilities::INVALID_OPENGL_ID;
	orig.indiciesID = Utilities::INVALID_OPENGL_ID;

	orig.transparentPositionsID = Utilities::INVALID_OPENGL_ID;
	orig.transparentTextCoordsID = Utilities::INVALID_OPENGL_ID;
	orig.transparentIndiciesID = Utilities::INVALID_OPENGL_ID;
}

VertexBuffer& VertexBuffer::operator=(VertexBuffer&& orig) noexcept
{
	positionsID = orig.positionsID;
	positions = std::move(orig.positions);
	textCoordsID = orig.textCoordsID;
	textCoords = std::move(orig.textCoords);
	indiciesID = orig.indiciesID;
	indicies = std::move(orig.indicies);
	transparentPositionsID = orig.transparentPositionsID;
	transparentPositions = std::move(orig.transparentPositions);
	transparentTextCoordsID = orig.transparentTextCoordsID;
	transparentTextCoords = std::move(orig.transparentTextCoords);
	transparentIndiciesID = orig.transparentIndiciesID;
	transparentIndicies = std::move(orig.transparentIndicies);

	orig.positionsID = Utilities::INVALID_OPENGL_ID;
	orig.textCoordsID = Utilities::INVALID_OPENGL_ID;
	orig.indiciesID = Utilities::INVALID_OPENGL_ID;

	orig.transparentPositionsID = Utilities::INVALID_OPENGL_ID;
	orig.transparentTextCoordsID = Utilities::INVALID_OPENGL_ID;
	orig.transparentIndiciesID = Utilities::INVALID_OPENGL_ID;

	return *this;
}

VertexBuffer::~VertexBuffer()
{
	assert(positionsID != Utilities::INVALID_OPENGL_ID);
	glDeleteBuffers(1, &positionsID);
	
	assert(textCoordsID != Utilities::INVALID_OPENGL_ID);
	glDeleteBuffers(1, &textCoordsID);
	
	assert(indiciesID != Utilities::INVALID_OPENGL_ID);
	glDeleteBuffers(1, &indiciesID);
	
	assert(transparentPositionsID != Utilities::INVALID_OPENGL_ID);
	glDeleteBuffers(1, &transparentPositionsID);
	
	assert(transparentTextCoordsID != Utilities::INVALID_OPENGL_ID);
	glDeleteBuffers(1, &transparentTextCoordsID);
	
	assert(transparentIndiciesID != Utilities::INVALID_OPENGL_ID);
	glDeleteBuffers(1, &transparentIndiciesID);
}

void VertexBuffer::clear()
{
	std::vector<glm::ivec3> newPositions;
	positions.swap(newPositions);

	std::vector<glm::ivec3> newTransparentPositions;
	transparentPositions.swap(newTransparentPositions);
	
	std::vector<glm::vec2> newTransparentTextCoords;
	transparentTextCoords.swap(newTransparentTextCoords);

	std::vector<glm::vec2> newTextCoords;
	textCoords.swap(newTextCoords);

	std::vector<unsigned int> newTransparentIndicies;
	transparentIndicies.swap(newTransparentIndicies);
	
	std::vector<unsigned int> newIndicies;
	indicies.swap(newIndicies);
}