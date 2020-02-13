#pragma once

#include <vector>

struct VertexBuffer
{
	std::vector<float> positions;
	std::vector<float> textCoords;
	std::vector<unsigned int> indicies;
};