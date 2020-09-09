#include "PickupManager.h"
#include "Player.h"
#include "Globals.h"
#include "ShaderHandler.h"
#include "GameMessenger.h"
#include "GameMessages.h"

PickupManager::PickupManager()
	: m_pickUps()
{
	GameMessenger::getInstance().subscribe<GameMessages::SpawnPickUp>(std::bind(
		&PickupManager::onSpawnPickUp, this, std::placeholders::_1), this);
	
	GameMessenger::getInstance().subscribe<GameMessages::PlayerDisgardPickup>(std::bind(
		&PickupManager::onPlayerDisgardPickup, this, std::placeholders::_1), this);
}

PickupManager::~PickupManager()
{
	GameMessenger::getInstance().unsubscribe<GameMessages::SpawnPickUp>(this);
	GameMessenger::getInstance().unsubscribe<GameMessages::PlayerDisgardPickup>(this);
}

void PickupManager::update(float deltaTime, const Player& player, std::mutex& chunkInteractionMutex, const ChunkManager& chunkManager)
{
	Rectangle visibilityRect = Globals::getVisibilityRect(player.getPosition());
	std::lock_guard<std::mutex> chunkInteractionLock(chunkInteractionMutex);
	for (auto pickup = m_pickUps.begin(); pickup != m_pickUps.end();)
	{
		const glm::vec3& pickupPosition = pickup->getPosition();
		if (!visibilityRect.contains({ pickupPosition.x, pickupPosition.z }))
		{
			pickup = m_pickUps.erase(pickup);
		}
		else if (pickup->isInReachOfPlayer(player.getMiddlePosition()))
		{
			GameMessenger::getInstance().broadcast<GameMessages::AddToInventory>({ pickup->getCubeType() });
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

void PickupManager::onPlayerDisgardPickup(const GameMessages::PlayerDisgardPickup& gameEvent)
{
	m_pickUps.emplace_back(gameEvent.cubeType, gameEvent.position, gameEvent.initialVelocity);
}

void PickupManager::onSpawnPickUp(const GameMessages::SpawnPickUp& gameEvent)
{
	m_pickUps.emplace_back(gameEvent.type, gameEvent.position);
}