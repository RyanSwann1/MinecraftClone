#pragma once

#include "glad.h"
#include "glm/glm.hpp"
#include "Utilities.h"
#include <vector>

struct VertexBuffer
{
	VertexBuffer()
		: positionsID(Utilities::INVALID_OPENGL_ID),
		textCoordsID(Utilities::INVALID_OPENGL_ID),
		indiciesID(Utilities::INVALID_OPENGL_ID)
	{}

	unsigned int positionsID =0 ;
	std::vector<glm::vec3> positions;
	unsigned int textCoordsID;
	std::vector<glm::vec2> textCoords;
	unsigned int indiciesID;
	std::vector<unsigned int> indicies;
	glm::vec3 m_owningChunkStartingPosition;
};