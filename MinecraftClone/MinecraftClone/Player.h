#pragma once

#include "NonCopyable.h"
#include "NonMovable.h"
#include "glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Globals.h"
#include "Inventory.h"
#include "Timer.h"
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
	InWater,
	OnGround,
};

class SelectedVoxelVisual;
class DestroyBlockVisual;
class Gui;
class Pickup;
class ChunkManager;
class Player : private NonCopyable, private NonMovable
{
public:
	Player();

	bool isUnderWater(const ChunkManager& chunkManager, std::mutex& playerMutex) const;
	const glm::vec3 getMiddlePosition() const;
	const glm::vec3& getPosition() const;
	const Camera& getCamera() const;
	const Inventory& getInventory() const;

	void resetDestroyCubeTimer();
	void addToInventory(eCubeType cubeType, Gui& gui);
	void spawn(const ChunkManager& chunkManager, std::mutex& playerMutex);
	void handleInputEvents(std::vector<Pickup>& pickUps, const sf::Event& currentSFMLEvent,
		ChunkManager& chunkManager, std::mutex& playerMutex, sf::Window& window, Gui& gui,
		SelectedVoxelVisual& selectedVoxel);
	void update(float deltaTime, std::mutex& playerMutex, ChunkManager& chunkManager, 
		DestroyBlockVisual& destroyBlockVisual, std::vector<Pickup>& pickUps, Gui& gui);

private:
	Camera m_camera;
	ePlayerState m_currentState;
	glm::vec3 m_position;
	glm::vec3 m_velocity;
	bool m_autoJump;
	bool m_requestingJump;
	sf::Clock m_jumpTimer;
	Inventory m_inventory;
	glm::ivec3 m_cubeToDestroyPosition;
	Timer m_placeCubeTimer;
	Timer m_destroyCubeTimer;

	void move(float deltaTime, const ChunkManager& chunkManager);
	void handleCollisions(const ChunkManager& chunkManager);
	void discardItem(std::vector<Pickup>& pickUps, Gui& gui);
	void placeBlock(ChunkManager& chunkManager, Gui& gui);
	void destroyFacingBlock(ChunkManager& chunkManager, std::vector<Pickup>& pickUps, DestroyBlockVisual& destroyBlockVisual);
	void handleAutoJump(const ChunkManager& chunkManager);
	void handleSelectedCube(const ChunkManager& chunkManager, SelectedVoxelVisual& selectedVoxel);
};