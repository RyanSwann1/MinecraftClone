#pragma once

#include "NonMovable.h"
#include "VertexArray.h"
#include "glm/glm.hpp"

namespace GameEvents
{
	struct SelectedCubeSetPosition;
	struct SelectedCubeSetActive;
};
class SelectedVoxelVisual : private NonCopyable, private NonMovable
{
public:
	SelectedVoxelVisual();
	~SelectedVoxelVisual();

	void render();

private:
	VertexArray m_mesh;
	glm::vec3 m_position;
	bool m_active;

	void onSetPosition(const GameEvents::SelectedCubeSetPosition& gameEvent);
	void onSetActive(const GameEvents::SelectedCubeSetActive& gameEvent);
};