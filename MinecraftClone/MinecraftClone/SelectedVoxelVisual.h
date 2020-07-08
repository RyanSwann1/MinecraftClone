#pragma once

#include "NonMovable.h"
#include "VertexArray.h"
#include "glm/glm.hpp"

class SelectedVoxelVisual : private NonCopyable, private NonMovable
{
public:
	SelectedVoxelVisual();

	void setPosition(const glm::vec3& position);
	void setActive(bool active);

	void render();

private:
	VertexArray m_mesh;
	glm::vec3 m_position;
	bool m_active;
};
