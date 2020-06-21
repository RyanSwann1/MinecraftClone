#pragma once

#include "NonCopyable.h"
#include "NonMovable.h"
#include "glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Globals.h"
#include "Item.h"
#include <mutex>
#include <SFML/Graphics.hpp>
#include <vector>

struct Camera : private NonCopyable, private NonMovable
{
	Camera();

	void move(const sf::Window& window);

	const float FOV;
	const float sensitivity;
	const float nearPlaneDistance;
	const float farPlaneDistance;
	glm::vec3 front;
	glm::vec3 right;
	glm::vec3 up;
	glm::vec2 rotation;
};

enum class ePlayerState
{
	Flying = 0,
	InAir,
	OnGround,
};

class PickUp;
class ChunkManager;
class Player : private NonCopyable, private NonMovable
{
public:
	Player();
	
	const glm::vec3 getMiddlePosition() const;
	const glm::vec3& getPosition() const;
	const Camera& getCamera() const;
	
	void addToInventory(eCubeType cubeType);
	void spawn(const ChunkManager& chunkManager, std::mutex& playerMutex);
	void handleInputEvents(std::vector<std::unique_ptr<PickUp>>& pickUps, const sf::Event& currentSFMLEvent,
		ChunkManager& chunkManager, std::mutex& playerMutex, sf::Window& window);
	void update(float deltaTime, std::mutex& playerMutex, const ChunkManager& chunkManager);

private:
	Camera m_camera;
	ePlayerState m_currentState;
	glm::vec3 m_position;
	glm::vec3 m_velocity;
	bool m_autoJump;
	sf::Clock m_jumpTimer;
	std::vector<Item> m_inventory;

	void move(float deltaTime, std::mutex& playerMutex, const ChunkManager& chunkManager);
	void handleCollisions(const ChunkManager& chunkManager);
	void toggleFlying();
	void toggleAutoJump();
	void discardItem(std::vector<std::unique_ptr<PickUp>>& pickUps);
	void placeBlock(ChunkManager& chunkManager, std::mutex& playerMutex);
	void destroyFacingBlock(ChunkManager& chunkManager, std::mutex& playerMutex, std::vector<std::unique_ptr<PickUp>>& pickUps) const;
	void moveCamera(const sf::Window& window);
	void handleAutoJump(const ChunkManager& chunkManager);
};