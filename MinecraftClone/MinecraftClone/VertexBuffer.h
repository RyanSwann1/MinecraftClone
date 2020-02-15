#pragma once

#include "glad.h"
#include "glm/glm.hpp"
#include <vector>

struct VertexBuffer
{
	void clear()
	{
		positions.clear();
		textCoords.clear();
		indicies.clear();
	}

	unsigned int positionsID;
	std::vector<float> positions;
	unsigned int textCoordsID;
	std::vector<float> textCoords;
	unsigned int indiciesID;
	std::vector<unsigned int> indicies;
	glm::vec3 m_owningChunkStartingPosition;
};