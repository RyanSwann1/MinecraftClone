#pragma once

#include "Globals.h"
#include "NonMovable.h"
#include "VertexArray.h"
#include "Timer.h"
#include "glm/glm.hpp"

enum class eCubeType;
class DestroyBlockVisual : private NonCopyable, private NonMovable
{
public:
	DestroyBlockVisual();

	bool isCompleted() const;

	void setPosition(const glm::ivec3& position);
	
	void reset();
	void update(float deltaTime);
	void render();

private:
	VertexArray m_mesh;
	Timer m_timer;
	eDestroyCubeIndex m_index;
	glm::ivec3 m_currentCubePosition;
};