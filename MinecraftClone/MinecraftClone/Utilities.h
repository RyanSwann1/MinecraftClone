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

	constexpr float WATER_ALPHA_VALUE = 0.5f;
	constexpr int SAND_MAX_HEIGHT = 17;
	constexpr int WATER_MAX_HEIGHT = 13;
	constexpr int STONE_MAX_HEIGHT = 12; 
	
	constexpr int CUBE_FACE_INDICIE_COUNT = 4;
	constexpr unsigned int INVALID_OPENGL_ID = 0;

	//constexpr int VISIBILITY_DISTANCE = 64;
	constexpr int VISIBILITY_DISTANCE = 480;

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
		glm::vec2(32.0f / 128.0f, 1.0f),
		glm::vec2(16.0f / 128.0f, 1.0f)
	};

	static constexpr std::array<glm::vec2, 4> SAND_TEXT_COORDS =
	{
		glm::vec2(32.0f / 128.0f, (128.0f - 16.0f) / 128.0f),
		glm::vec2(48.0f / 128.0f, (128.0f - 16.0f) / 128.0f),
		glm::vec2(48.0f / 128.0f, 1.0f),
		glm::vec2(32.0f / 128.0f, 1.0f)
	};

	static constexpr std::array<glm::vec2, 4> STONE_TEXT_COORDS =
	{
		glm::vec2(48.0f / 128.0f, (128.0f - 16.0f) / 128.0f),
		glm::vec2(64.0f / 128.0f, (128.0f - 16.0f) / 128.0f),
		glm::vec2(64.0f / 128.0f, 1.0f),
		glm::vec2(48.0f / 128.0f, 1.0f)
	};

	static constexpr std::array<glm::vec2, 4> WATER_TEXT_COORDS =
	{
		glm::vec2(64.0f / 128.0f, (128.0f - 16.0f) / 128.0f),
		glm::vec2(80.0f / 128.0f, (128.0f - 16.0f) / 128.0f),
		glm::vec2(80.0f / 128.0f, 1.0f),
		glm::vec2(64.0f / 128.0f, 1.0f)
	};

	static constexpr std::array<glm::vec3, 4> CUBE_FACE_FRONT = { glm::vec3(0, 0, 1.0), glm::vec3(1.0, 0, 1.0), glm::vec3(1.0, 1.0, 1.0), glm::vec3(0, 1.0, 1.0) };
	static constexpr std::array<glm::vec3, 4> CUBE_FACE_BACK = { glm::vec3(0, 0, 0), glm::vec3(1.0, 0, 0), glm::vec3(1.0, 1.0, 0), glm::vec3(0, 1.0, 0) };

	static constexpr std::array<glm::vec3, 4> CUBE_FACE_LEFT = { glm::vec3(0, 0, 0), glm::vec3(0, 0, 1.0), glm::vec3(0, 1.0, 1.0), glm::vec3(0, 1.0, 0) };
	static constexpr std::array<glm::vec3, 4> CUBE_FACE_RIGHT = { glm::vec3(1.0, 0, 0), glm::vec3(1.0, 0, 1.0), glm::vec3(1.0, 1.0, 1.0), glm::vec3(1.0, 1.0, 0) };

	static constexpr std::array<glm::vec3, 4> CUBE_FACE_TOP = { glm::vec3(0, 1.0, 0), glm::vec3(0, 1.0, 1.0), glm::vec3(1.0, 1.0, 1.0), glm::vec3(1.0, 1.0, 0) };
	static constexpr std::array<glm::vec3, 4> CUBE_FACE_BOTTOM = { glm::vec3(0, 0, 0), glm::vec3(0, 0, 1.0), glm::vec3(1.0, 0, 1.0), glm::vec3(1.0, 0, 0) };
	
	//inline glm::vec2 getClosestChunkStartingPosition(glm::vec2 position)
	//{
	//	glm::vec2 positionOnGrid(glm::vec2((position.x / CHUNK_WIDTH) * CHUNK_WIDTH, (position.y / CHUNK_DEPTH) * CHUNK_DEPTH));
	//	if (position.x < 0 && static_cast<int>(position.x) % CHUNK_WIDTH < 0)
	//	{
	//		positionOnGrid.x += std::abs(static_cast<int>(position.x) % CHUNK_WIDTH);
	//		positionOnGrid.x -= CHUNK_WIDTH;
	//	}
	//	else if (position.x > 0 && static_cast<int>(position.x) % CHUNK_WIDTH > 0)
	//	{
	//		positionOnGrid.x -= std::abs(static_cast<int>(position.x) % CHUNK_WIDTH);
	//	}
	//	if (position.y < 0 && static_cast<int>(position.y) % CHUNK_DEPTH < 0)
	//	{
	//		positionOnGrid.y += std::abs(static_cast<int>(position.y) % CHUNK_DEPTH);
	//		positionOnGrid.y -= CHUNK_DEPTH;
	//	}
	//	else if (position.y > 0 && static_cast<int>(position.y) % CHUNK_DEPTH > 0)
	//	{
	//		positionOnGrid.y -= std::abs(static_cast<int>(position.y) % CHUNK_DEPTH);
	//	}

	//	return positionOnGrid;
	//}

	inline glm::ivec3 getClosestChunkStartingPosition(glm::ivec3 position)
	{
		if (position.x < 0 && position.x % CHUNK_WIDTH < 0)
		{
			position.x += std::abs(position.x % CHUNK_WIDTH);
			position.x -= CHUNK_WIDTH;
		}
		else if (position.x > 0 && position.x % CHUNK_WIDTH > 0)
		{
			position.x -= std::abs(position.x % CHUNK_WIDTH);
		}
		if (position.z < 0 && position.z % CHUNK_DEPTH < 0)
		{
			position.z += std::abs(position.z % CHUNK_DEPTH);
			position.z -= CHUNK_DEPTH;
		}
		else if (position.z > 0 && position.z % CHUNK_DEPTH > 0)
		{
			position.z -= std::abs(position.z % CHUNK_DEPTH);
		}

		return glm::ivec3(position.x, 0, position.z);
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
}