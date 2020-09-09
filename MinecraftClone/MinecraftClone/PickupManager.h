#pragma once

#include "Globals.h"
#include "NonCopyable.h"
#include "NonMovable.h"
#include "Pickup.h"
#include <mutex>

class Frustum;
class ShaderHandler;
class Player;
namespace GameMessages
{
	struct PlayerDisgardPickup;
	struct SpawnPickUp;
}
class PickupManager : private NonCopyable, private NonMovable
{
public:
	PickupManager();
	~PickupManager();

	void update(float deltaTime, const Player& player, std::mutex& chunkInteractionMutex, const ChunkManager& chunkManager);
	void render(const Frustum& frustum, ShaderHandler& shaderHandler, const glm::mat4& view, const glm::mat4& projection);

private:
	std::vector<Pickup> m_pickUps;

	void onPlayerDisgardPickup(const GameMessages::PlayerDisgardPickup& gameEvent);
	void onSpawnPickUp(const GameMessages::SpawnPickUp& gameEvent);
};