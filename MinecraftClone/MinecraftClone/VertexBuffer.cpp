#include "VertexBuffer.h"

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
{}

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
}

void VertexBuffer::clear()
{
	positionsID = Utilities::INVALID_OPENGL_ID;
	positions.clear();

	transparentPositionsID = Utilities::INVALID_OPENGL_ID;
	transparentPositions.clear();

	transparentTextCoordsID = Utilities::INVALID_OPENGL_ID;
	transparentTextCoords.clear();

	textCoordsID = Utilities::INVALID_OPENGL_ID;
	textCoords.clear();

	transparentIndiciesID = Utilities::INVALID_OPENGL_ID;
	transparentIndicies.clear();

	indiciesID = Utilities::INVALID_OPENGL_ID;
	indicies.clear();
}