#pragma once

#include "glad.h"
#include "glm/glm.hpp"
#include "Globals.h"
#include "NonCopyable.h"
#include <vector>

struct VertexArray;
struct VertexBuffer : private NonCopyable
{
	VertexBuffer();
	VertexBuffer(VertexBuffer&&) noexcept;
	VertexBuffer& operator=(VertexBuffer&&) noexcept;
	~VertexBuffer();

	void bind();
	void clear();

	int elementBufferIndex;
	bool bindToVAO;
	bool displayable;
	unsigned int positionsID;
	std::vector<glm::vec3> positions;
	unsigned int lightIntensityID;
	std::vector<float> lightIntensityVertices;
	unsigned int textCoordsID;
	std::vector<glm::vec3> textCoords;
	unsigned int indiciesID;
	std::vector<unsigned int> indicies;

private:
	void onDestroy();
};