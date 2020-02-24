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

	bool active = true;
	unsigned int positionsID;
	std::vector<glm::vec3> positions;
	unsigned int textCoordsID;
	std::vector<glm::vec2> textCoords;
	unsigned int indiciesID;
	std::vector<unsigned int> indicies;
	glm::vec3 m_owningChunkStartingPosition;
};