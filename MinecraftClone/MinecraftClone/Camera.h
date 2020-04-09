#pragma once

#include <SFML/Graphics.hpp>
#include "glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

//
//LEARNOPENGL.COM CAMERA
//

struct Camera
{
	Camera(glm::vec3 startPosition);

	void move(float deltaTime);

	void mouse_callback(double xpos, double ypos);

	float m_speed;
	glm::vec3 m_position;
	glm::vec3 m_front;
	glm::vec3 m_up;

	float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
	float pitch = 0.0f;
	//	sf::Vector2i windowSize(1980, 1080);
	float lastX = 1980.0f / 2.0;
	float lastY = 1080.0 / 2.0;
	float fov = 45.0f;
};