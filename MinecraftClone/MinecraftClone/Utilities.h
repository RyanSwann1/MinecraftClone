#pragma once

#include "glm/glm.hpp"
#include <array>
#include <vector>

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
	constexpr int CHUNK_WIDTH = 32;
	constexpr int CHUNK_HEIGHT = 64;
	constexpr int CHUNK_DEPTH = 32;
	constexpr int CHUNK_SIZE = (CHUNK_WIDTH + CHUNK_DEPTH) / 2.0f;
	constexpr int STONE_MAX_HEIGHT = 10;
	constexpr int DIRT_MAX_HEIGHT = 14;
	constexpr int CUBE_FACE_INDICIE_COUNT = 4;
	constexpr unsigned int INVALID_OPENGL_ID = 0;
	constexpr int VISIBILITY_DISTANCE = 320;

	static constexpr std::array<glm::vec2, 4> GRASS_TEXT_COORDS =
	{	
		glm::vec2(0.0f, (128.0f - 16.0f) / 128.0f),
		glm::vec2(16.0f / 128.0f, (128.0f - 16.0f) / 128.0f),
		glm::vec2(16.0f / 128.0f, 1.0f),
		glm::vec2(0.0f, 1.0f)
	};

	static constexpr std::array<glm::vec2, 4> GRASS_SIDE_TEXT_COORDS =
	{
		glm::vec2(16.0f / 128.0f, (128.0f - 16.0f) / 128.0f),
		glm::vec2(32.0f / 128.0f, (128.0f - 16.0f) / 128.0f),
		glm::vec2(32.0f / 128.0f, 1),
		glm::vec2(16.0f / 128.0f, 1)
	};

	static constexpr std::array<glm::vec2, 4> DIRT_TEXT_COORDS =
	{
		glm::vec2(32.0f / 128.0f, (128.0f - 16.0f) / 128.0f),
		glm::vec2(48.0f / 128.0f, (128.0f - 16.0f) / 128.0f),
		glm::vec2(48.0f / 128.0f, 1),
		glm::vec2(32.0f / 128.0f, 1)
	};

	static constexpr std::array<glm::vec2, 4> STONE_TEXT_COORDS =
	{
		glm::vec2(48.0f / 128.0f, (128.0f - 16.0f) / 128.0f),
		glm::vec2(64.0f / 128.0f, (128.0f - 16.0f) / 128.0f),
		glm::vec2(64.0f / 128.0f, 1),
		glm::vec2(48.0f / 128.0f, 1)
	};

	static constexpr std::array<glm::vec3, 4> CUBE_FACE_FRONT = { glm::vec3(0, 0, 1.0), glm::vec3(1.0, 0, 1.0), glm::vec3(1.0, 1.0, 1.0), glm::vec3(0, 1.0, 1.0) };
	static constexpr std::array<glm::vec3, 4> CUBE_FACE_BACK = { glm::vec3(0, 0, 0), glm::vec3(1.0, 0, 0), glm::vec3(1.0, 1.0, 0), glm::vec3(0, 1.0, 0) };

	static constexpr std::array<glm::vec3, 4> CUBE_FACE_LEFT = { glm::vec3(0, 0, 0), glm::vec3(0, 0, 1.0), glm::vec3(0, 1.0, 1.0), glm::vec3(0, 1.0, 0) };
	static constexpr std::array<glm::vec3, 4> CUBE_FACE_RIGHT = { glm::vec3(1.0, 0, 0), glm::vec3(1.0, 0, 1.0), glm::vec3(1.0, 1.0, 1.0), glm::vec3(1.0, 1.0, 0) };

	static constexpr std::array<glm::vec3, 4> CUBE_FACE_TOP = { glm::vec3(0, 1.0, 0), glm::vec3(0, 1.0, 1.0), glm::vec3(1.0, 1.0, 1.0), glm::vec3(1.0, 1.0, 0) };
	static constexpr std::array<glm::vec3, 4> CUBE_FACE_BOTTOM = { glm::vec3(0, 0, 0), glm::vec3(0, 0, 1.0), glm::vec3(1.0, 0, 1.0), glm::vec3(1.0, 0, 0) };

	inline glm::vec2 getClosestChunkStartingPosition(glm::vec2 position)
	{
		glm::vec2 positionOnGrid(glm::vec2((position.x / CHUNK_WIDTH) * CHUNK_WIDTH, (position.y / CHUNK_DEPTH) * CHUNK_DEPTH));
		if (position.x < 0 && static_cast<int>(position.x) % CHUNK_WIDTH < 0)
		{
			positionOnGrid.x += std::abs(static_cast<int>(position.x) % CHUNK_WIDTH);
			positionOnGrid.x -= CHUNK_WIDTH;
		}
		else if (position.x > 0 && static_cast<int>(position.x) % CHUNK_WIDTH > 0)
		{
			positionOnGrid.x -= std::abs(static_cast<int>(position.x) % CHUNK_WIDTH);
		}
		if (position.y < 0 && static_cast<int>(position.y) % CHUNK_DEPTH < 0)
		{
			positionOnGrid.y += std::abs(static_cast<int>(position.y) % CHUNK_DEPTH);
			positionOnGrid.y -= CHUNK_DEPTH;
		}
		else if (position.y > 0 && static_cast<int>(position.y) % CHUNK_DEPTH > 0)
		{
			positionOnGrid.y -= std::abs(static_cast<int>(position.y) % CHUNK_DEPTH);
		}

		return positionOnGrid;
	}

	inline float clampTo(float value, float min, float max)
	{
		if (value < min)
		{
			value = min;
		}
		else if (value > max)
		{
			value = max;
		}

		return value;
	}

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