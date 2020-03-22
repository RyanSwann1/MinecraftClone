#pragma once

#include "glad.h"
#include "glm/glm.hpp"
#include "Utilities.h"
#include "NonCopyable.h"
#include <vector>

struct VertexBuffer : private NonCopyable
{
	VertexBuffer();
	VertexBuffer(VertexBuffer&&) noexcept;
	VertexBuffer& operator=(VertexBuffer&&) noexcept;
	~VertexBuffer();

	void clearOpaqueVertices();
	void clearTransparentVertices();

	void clear();

	//Opaque
	unsigned int positionsID;
	std::vector<glm::ivec3> positions;

	unsigned int textCoordsID;
	std::vector<glm::vec2> textCoords;

	unsigned int indiciesID;
	std::vector<unsigned int> indicies;
	
	//Transparent
	unsigned int transparentPositionsID;
	std::vector<glm::ivec3> transparentPositions;

	unsigned int transparentTextCoordsID;
	std::vector<glm::vec2> transparentTextCoords;

	unsigned int transparentIndiciesID;
	std::vector<unsigned int> transparentIndicies;
};