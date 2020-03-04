#pragma once

#include "glm/glm.hpp"
#include <array>
#include <vector>
#include <random>

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

enum class eDirection
{
	Left = 0,
	Right,
	Forward,
	Back,
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
	constexpr int TREE_SPAWN_CHANCE = 1400;
	constexpr int TREE_MIN_HEIGHT = 9;
	constexpr int TREE_MAX_HEIGHT = 9;
	constexpr int MAX_TREE_PER_CHUNK = 3;
	
	constexpr int CUBE_FACE_INDICIE_COUNT = 4;
	constexpr unsigned int INVALID_OPENGL_ID = 0;

	//constexpr int VISIBILITY_DISTANCE = 64;
	constexpr int VISIBILITY_DISTANCE = 480;

	constexpr std::array<int, 6> LEAVES_DISTANCES =
	{
		3, 3, 2, 2, 1, 1
	};

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

	static constexpr std::array<glm::vec2, 4> TREESTUMP_TEXT_COORDS =
	{
		glm::vec2(80.0f / 128.0f, (128.0f - 16.0f) / 128.0f),
		glm::vec2(96.0f / 128.0f, (128.0f - 16.0f) / 128.0f),
		glm::vec2(96.0f / 128.0f, 1.0f),
		glm::vec2(80.0f / 128.0f, 1.0f)
	};

	static constexpr std::array<glm::vec2, 4> LEAVES_TEXT_COORDS =
	{
		glm::vec2(96.0f / 128.0f, (128.0f - 16.0f) / 128.0f),
		glm::vec2(112.0f / 128.0f, (128.0f - 16.0f) / 128.0f),
		glm::vec2(112.0f / 128.0f, 1.0f),
		glm::vec2(96.0f / 128.0f, 1.0f)
	};

	static constexpr std::array<glm::vec3, 4> CUBE_FACE_FRONT = { glm::vec3(0, 0, 1.0), glm::vec3(1.0, 0, 1.0), glm::vec3(1.0, 1.0, 1.0), glm::vec3(0, 1.0, 1.0) };
	static constexpr std::array<glm::vec3, 4> CUBE_FACE_BACK = { glm::vec3(0, 0, 0), glm::vec3(1.0, 0, 0), glm::vec3(1.0, 1.0, 0), glm::vec3(0, 1.0, 0) };

	static constexpr std::array<glm::vec3, 4> CUBE_FACE_LEFT = { glm::vec3(0, 0, 0), glm::vec3(0, 0, 1.0), glm::vec3(0, 1.0, 1.0), glm::vec3(0, 1.0, 0) };
	static constexpr std::array<glm::vec3, 4> CUBE_FACE_RIGHT = { glm::vec3(1.0, 0, 0), glm::vec3(1.0, 0, 1.0), glm::vec3(1.0, 1.0, 1.0), glm::vec3(1.0, 1.0, 0) };

	static constexpr std::array<glm::vec3, 4> CUBE_FACE_TOP = { glm::vec3(0, 1.0, 0), glm::vec3(0, 1.0, 1.0), glm::vec3(1.0, 1.0, 1.0), glm::vec3(1.0, 1.0, 0) };
	static constexpr std::array<glm::vec3, 4> CUBE_FACE_BOTTOM = { glm::vec3(0, 0, 0), glm::vec3(0, 0, 1.0), glm::vec3(1.0, 0, 1.0), glm::vec3(1.0, 0, 0) };
	
	inline glm::ivec3 convertToWorldPosition(glm::ivec3& position, const glm::ivec3& chunkStartingPosition)
	{
		return glm::ivec3(chunkStartingPosition.x + position.x, position.y, chunkStartingPosition.z + position.z);
	}

	inline glm::ivec3 getClosestChunkStartingPosition(const glm::ivec3& position)
	{
		glm::ivec3 closestStartingPosition(position);
		
		if (position.x < 0 && position.x % CHUNK_WIDTH < 0)
		{
			closestStartingPosition.x += std::abs(position.x % CHUNK_WIDTH);
			closestStartingPosition.x -= CHUNK_WIDTH;
		}
		else if (position.x > 0 && position.x % CHUNK_WIDTH > 0)
		{
			closestStartingPosition.x -= std::abs(position.x % CHUNK_WIDTH);
		}
		if (position.z < 0 && position.z % CHUNK_DEPTH < 0)
		{
			closestStartingPosition.z += std::abs(position.z % CHUNK_DEPTH);
			closestStartingPosition.z -= CHUNK_DEPTH;
		}
		else if (position.z > 0 && position.z % CHUNK_DEPTH > 0)
		{
			closestStartingPosition.z -= std::abs(position.z % CHUNK_DEPTH);
		}

		return glm::ivec3(closestStartingPosition.x, 0, closestStartingPosition.z);
	}

	inline int getRandomNumber(int min, int max)
	{
		static std::random_device rd;  //Will be used to obtain a seed for the random number engine
		static std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
		std::uniform_int_distribution<> dis(min, max);

		return dis(gen);
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