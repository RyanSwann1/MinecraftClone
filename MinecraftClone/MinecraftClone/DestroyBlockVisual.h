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

	void set(const glm::vec3& position, float expirationTime);
	void reset();
	void update(float deltaTime);
	void render();

private:
	VertexArray m_mesh;
	Timer m_timer;
	eDestroyCubeIndex m_index;
	glm::ivec3 m_currentCubePosition;
};