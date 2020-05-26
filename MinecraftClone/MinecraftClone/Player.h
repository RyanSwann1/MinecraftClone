#pragma once

#include "NonCopyable.h"
#include "NonMovable.h"
#include "glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <mutex>
#include <SFML/Graphics.hpp>

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
	float yaw;
	float pitch;
};

class Player : private NonCopyable, private NonMovable
{
public:
	Player();
	
	const glm::vec3& getPosition() const;
	const Camera& getCamera() const;

	void reset(glm::vec3& playerPositionOnChunkGeneration);
	void moveCamera(const sf::Window& window);
	void move(float deltaTime, std::mutex& playerMutex, glm::vec3& playerPositionOnChunkGeneration);

private:
	Camera m_camera;
	glm::vec3 m_position;
	float m_movementSpeed;
};