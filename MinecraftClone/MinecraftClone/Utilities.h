#pragma once

#include "glm/glm.hpp"
#include <array>

enum class eCubeSide
{
	Front,
	Back,
	Left,
	Right,
	Top,
	Bottom,
	Total
};

namespace Utilities
{
	constexpr int CHUNK_WIDTH = 16;
	constexpr int CHUNK_HEIGHT = 16;
	constexpr int CHUNK_DEPTH = 16;
	constexpr int STONE_MAX_HEIGHT = 10;
	constexpr int DIRT_MAX_HEIGHT = 14;
	constexpr int CUBE_FACE_INDICIE_COUNT = 4;
	constexpr int INVALID_OPENGL_ID = -1;
	constexpr int VISIBILITY_DISTANCE = 36;

	glm::ivec2 get2DCentrePosition(glm::vec3 startingPosition, glm::vec3 endingPosition)
	{
		glm::ivec2 position;
		position.x = (endingPosition.x - startingPosition.x) / 2.0f;
		position.y = (endingPosition.y - startingPosition.y) / 2.0f;

		return position;
	}

	static constexpr std::array<glm::vec3, 4> CUBE_FACE_FRONT = { glm::vec3(0, 0, 1.0), glm::vec3(1.0, 0, 1.0), glm::vec3(1.0, 1.0, 1.0), glm::vec3(0, 1.0, 1.0) };
	static constexpr std::array<glm::vec3, 4> CUBE_FACE_BACK = { glm::vec3(0, 0, 0), glm::vec3(1.0, 0, 0), glm::vec3(1.0, 1.0, 0), glm::vec3(0, 1.0, 0) };

	static constexpr std::array<glm::vec3, 4> CUBE_FACE_LEFT = { glm::vec3(0, 0, 0), glm::vec3(0, 0, 1.0), glm::vec3(0, 1.0, 1.0), glm::vec3(0, 1.0, 0) };
	static constexpr std::array<glm::vec3, 4> CUBE_FACE_RIGHT = { glm::vec3(1.0, 0, 0), glm::vec3(1.0, 0, 1.0), glm::vec3(1.0, 1.0, 1.0), glm::vec3(1.0, 1.0, 0) };

	static constexpr std::array<glm::vec3, 4> CUBE_FACE_TOP = { glm::vec3(0, 1.0, 0), glm::vec3(0, 1.0, 1.0), glm::vec3(1.0, 1.0, 1.0), glm::vec3(1.0, 1.0, 0) };
	static constexpr std::array<glm::vec3, 4> CUBE_FACE_BOTTOM = { glm::vec3(0, 0, 0), glm::vec3(0, 0, 1.0), glm::vec3(1.0, 0, 1.0), glm::vec3(1.0, 0, 0) };

	static const std::array<unsigned int, 6> CUBE_FACE_INDICIES =
	{
		0, 1, 2,
		2, 3, 0
	};

	static const std::array<unsigned int, 36> CUBE_INDICIES =
	{
		0, 1, 2,
		2, 3, 0,

		4, 5, 6,
		6, 7, 4,

		8, 9, 10,
		10, 11, 8,

		12, 13, 14,
		14, 15, 12,

		16, 17, 18,
		18, 19, 16,

		20, 21, 22,
		22, 23, 20
	};
}