#pragma once

#include "glad.h"
#include "glm/glm.hpp"
#include "Utilities.h"
#include <vector>

struct VertexBuffer
{
	VertexBuffer()
		: opaqueVerticesID(Utilities::INVALID_OPENGL_ID),
		opaqueVertices(),
		transparentVerticesID(Utilities::INVALID_OPENGL_ID),
		transparentVertices(),
		transparentIndiciesID(Utilities::INVALID_OPENGL_ID),
		transparentIndicies(),
		indiciesID(Utilities::INVALID_OPENGL_ID),
		indicies()
	{}

	void clear()
	{
		opaqueVerticesID = Utilities::INVALID_OPENGL_ID;
		opaqueVertices.clear();

		transparentVerticesID = Utilities::INVALID_OPENGL_ID;
		transparentVertices.clear();

		transparentIndiciesID = Utilities::INVALID_OPENGL_ID;
		transparentIndicies.clear();
		
		indiciesID = Utilities::INVALID_OPENGL_ID;
		indicies.clear();
	}

	unsigned int opaqueVerticesID;
	std::vector<float> opaqueVertices;

	unsigned int transparentVerticesID;
	std::vector<float> transparentVertices;
	
	unsigned int transparentIndiciesID;
	std::vector<unsigned int> transparentIndicies;

	unsigned int indiciesID;
	std::vector<unsigned int> indicies;
};