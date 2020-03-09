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
		indiciesID(Utilities::INVALID_OPENGL_ID),
		transparentPositionsID(Utilities::INVALID_OPENGL_ID),
		transparentTextCoordsID(Utilities::INVALID_OPENGL_ID),
		transparentIndiciesID(Utilities::INVALID_OPENGL_ID)
	{}

	void clear()
	{


		transparentIndiciesID = Utilities::INVALID_OPENGL_ID;
		transparentIndicies.clear();
		
		indiciesID = Utilities::INVALID_OPENGL_ID;
		indicies.clear();
	}

	unsigned int opaqueVerticesID;
	std::vector<float> opaqueVertices;

	unsigned int transparentVerticesID;
	std::vector<float> transparentVertices;

	//unsigned int positionsID;
	//std::vector<glm::vec3> positions;
	//
	//unsigned int transparentPositionsID;
	//std::vector<glm::vec3> transparentPositions;

	//unsigned int transparentTextCoordsID;
	//std::vector<glm::vec2> transparentTextCoords;

	//unsigned int textCoordsID;
	//std::vector<glm::vec2> textCoords;
	
	unsigned int transparentIndiciesID;
	std::vector<unsigned int> transparentIndicies;

	unsigned int indiciesID;
	std::vector<unsigned int> indicies;
};