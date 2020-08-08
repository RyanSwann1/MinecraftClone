#pragma once

#include "Globals.h"
#include "NonMovable.h"
#include "VertexArray.h"
#include "Timer.h"
#include "glm/glm.hpp"

namespace GameEvents
{
	struct DestroyCubeReset;
	struct DestroyCubeSetPosition;
}
enum class eCubeType;
class DestroyBlockVisual : private NonCopyable, private NonMovable
{
public:
	DestroyBlockVisual();
	~DestroyBlockVisual();

	void reset();
	void update(float deltaTime);
	void render();

private:
	VertexArray m_mesh;
	Timer m_timer;
	eDestroyCubeIndex m_index;
	glm::ivec3 m_currentCubePosition;

	void onSetPosition(const GameEvents::DestroyCubeSetPosition& gameEvent);
	void onReset(const GameEvents::DestroyCubeReset& gameEvent);
};