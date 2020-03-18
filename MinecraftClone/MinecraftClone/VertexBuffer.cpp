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
	positions(orig.positions),
	textCoordsID(orig.textCoordsID),
	textCoords(orig.textCoords),
	indiciesID(orig.indiciesID),
	indicies(orig.indicies),
	transparentPositionsID(orig.transparentPositionsID),
	transparentPositions(orig.transparentPositions),
	transparentTextCoordsID(orig.transparentTextCoordsID),
	transparentTextCoords(orig.transparentTextCoords),
	transparentIndiciesID(orig.transparentIndiciesID),
	transparentIndicies(orig.transparentIndicies)
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
	positions = orig.positions;
	textCoordsID = orig.textCoordsID;
	textCoords = orig.textCoords;
	indiciesID = orig.indiciesID;
	indicies = orig.indicies;
	transparentPositionsID = orig.transparentPositionsID;
	transparentPositions = orig.transparentPositions;
	transparentTextCoordsID = orig.transparentTextCoordsID;
	transparentTextCoords = orig.transparentTextCoords;
	transparentIndiciesID = orig.transparentIndiciesID;
	transparentIndicies = orig.transparentIndicies;

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
	if (positionsID != Utilities::INVALID_OPENGL_ID)
	{
		glDeleteBuffers(1, &positionsID);
	}

	if (textCoordsID != Utilities::INVALID_OPENGL_ID)
	{
		glDeleteBuffers(1, &textCoordsID);
	}

	if (indiciesID != Utilities::INVALID_OPENGL_ID)
	{
		glDeleteBuffers(1, &indiciesID);
	}

	if (transparentPositionsID != Utilities::INVALID_OPENGL_ID)
	{
		glDeleteBuffers(1, &transparentPositionsID);
	}

	if (transparentTextCoordsID != Utilities::INVALID_OPENGL_ID)
	{
		glDeleteBuffers(1, &transparentTextCoordsID);
	}

	if (transparentIndiciesID != Utilities::INVALID_OPENGL_ID)
	{
		glDeleteBuffers(1, &transparentIndiciesID);
	}
}

void VertexBuffer::clear()
{
	//positionsID = Utilities::INVALID_OPENGL_ID;
	positions.clear();

	//transparentPositionsID = Utilities::INVALID_OPENGL_ID;
	transparentPositions.clear();

	//transparentTextCoordsID = Utilities::INVALID_OPENGL_ID;
	transparentTextCoords.clear();

	//textCoordsID = Utilities::INVALID_OPENGL_ID;
	textCoords.clear();

	//transparentIndiciesID = Utilities::INVALID_OPENGL_ID;
	transparentIndicies.clear();

	//indiciesID = Utilities::INVALID_OPENGL_ID;
	indicies.clear();
}