#pragma once

#include "Globals.h"
#include "NonMovable.h"
#include "VertexArray.h"
#include "Timer.h"
#include "glm/glm.hpp"

namespace GameMessages
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

	void onSetPosition(const GameMessages::DestroyCubeSetPosition& gameMessage);
	void onReset(const GameMessages::DestroyCubeReset& gameMessage);
};