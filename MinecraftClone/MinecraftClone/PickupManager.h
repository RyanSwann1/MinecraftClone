#pragma once

#include "Globals.h"
#include "NonCopyable.h"
#include "NonMovable.h"
#include "Pickup.h"
#include <mutex>

class Frustum;
class ShaderHandler;
class Player;
class PickupManager : private NonCopyable, private NonMovable
{
public:
	PickupManager();
	~PickupManager();

	void update(float deltaTime, const Player& player, std::mutex& playerMutex, const ChunkManager& chunkManager);
	void render(const Frustum& frustum, ShaderHandler& shaderHandler, const glm::mat4& view, const glm::mat4& projection);

private:
	std::vector<Pickup> m_pickUps;

	void onSpawnPickUp(const void* gameEvent);
};