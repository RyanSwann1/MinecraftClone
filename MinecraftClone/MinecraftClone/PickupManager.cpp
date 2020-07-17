#include "PickupManager.h"
#include "Player.h"
#include "Globals.h"
#include "ShaderHandler.h"
#include "GameEventMessenger.h"
#include "GameEvents.h"

PickupManager::PickupManager()
	: m_pickUps()
{
	GameEventMessenger::getInstance().subscribe(eGameEventType::SpawnPickup, std::bind(&PickupManager::onSpawnPickUp, this, std::placeholders::_1), this);
}

PickupManager::~PickupManager()
{
	GameEventMessenger::getInstance().unsubscribe(eGameEventType::SpawnPickup, this);
}

void PickupManager::update(float deltaTime, const Player& player, std::mutex& playerMutex, const ChunkManager& chunkManager)
{
	Rectangle visibilityRect = Globals::getVisibilityRect(player.getPosition());
	std::lock_guard<std::mutex> playerLock(playerMutex);
	for (auto pickup = m_pickUps.begin(); pickup != m_pickUps.end();)
	{
		const glm::vec3& pickupPosition = pickup->getPosition();
		if (!visibilityRect.contains({ pickupPosition.x, pickupPosition.z }))
		{
			pickup = m_pickUps.erase(pickup);
		}
		else if (pickup->isInReachOfPlayer(player.getMiddlePosition()) && player.getInventory().isItemAddable(pickup->getCubeType()))
		{
			//player.addToInventory(pickup->getCubeType(), gui);
			pickup = m_pickUps.erase(pickup);
		}
		else
		{
			pickup->update(player, deltaTime, chunkManager);
			++pickup;
		}
	}
}

void PickupManager::render(const Frustum& frustum, ShaderHandler& shaderHandler, const glm::mat4& view, const glm::mat4& projection)
{
	shaderHandler.switchToShader(eShaderType::Pickup);
	shaderHandler.setUniformMat4f(eShaderType::Pickup, "uView", view);
	shaderHandler.setUniformMat4f(eShaderType::Pickup, "uProjection", projection);

	for (auto& pickUp : m_pickUps)
	{
		pickUp.render(frustum, shaderHandler);
	}
}

void PickupManager::onSpawnPickUp(const void* gameEvent)
{
	const GameEvents::SpawnPickUp* spawnPickup = static_cast<const GameEvents::SpawnPickUp*>(gameEvent);
	assert(spawnPickup);
	
	m_pickUps.emplace_back(spawnPickup->type, spawnPickup->position);
}