#pragma once

#include "glm/glm.hpp"
#include "CubeID.h"
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
	Left,
	Right,
	Up,
	Down,
	Forward,
	Back
};

//https://stackoverflow.com/questions/4422507/superiority-of-unnamed-namespace-over-static

namespace Utilities
{
	constexpr int CHUNK_WIDTH = 32;
	constexpr int CHUNK_HEIGHT = 160;
	constexpr int CHUNK_DEPTH = 32;
	constexpr int CHUNK_VOLUME = CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_DEPTH;

	constexpr float WATER_ALPHA_VALUE = 0.5f;
	constexpr int SAND_MAX_HEIGHT = 17;
	constexpr int WATER_MAX_HEIGHT = 13;
	constexpr int STONE_MAX_HEIGHT = 12; 
	constexpr int TREE_SPAWN_CHANCE = 1400;
	constexpr int CACTUS_SPAWN_CHANCE = 1000;
	constexpr int TREE_HEIGHT = 9;
	constexpr int CACTUS_MIN_HEIGHT = 1;
	constexpr int CACTUS_MAX_HEIGHT = 4;
	constexpr int MAX_TREE_PER_CHUNK = 3;
	constexpr int MAX_CACTUS_PER_CHUNK = 4;
	constexpr int MAX_LEAVES_DISTANCE = 3;
	
	constexpr int CUBE_FACE_INDICIE_COUNT = 4;
	constexpr unsigned int INVALID_OPENGL_ID = 0;

	constexpr float MOISTURE_LACUNARITY = 0.5f;
	constexpr float MOISTURE_PERSISTENCE = 0.5f;
	constexpr int MOISTURE_OCTAVES = 3;

	constexpr float TERRAIN_LACUNARITY = 15.f;
	constexpr float TERRAIN_PERSISTENCE = 0.5f;
	constexpr int TERRAIN_OCTAVES = 8;

	constexpr int VISIBILITY_DISTANCE = 640;
	constexpr int MAP_SIZE = 8000;
	const glm::vec3 PLAYER_STARTING_POSITION(0.0f, 250.f, 0.0f);

	constexpr std::array<int, 6> LEAVES_DISTANCES =
	{
		MAX_LEAVES_DISTANCE, 
		MAX_LEAVES_DISTANCE, 
		MAX_LEAVES_DISTANCE - 1, 
		MAX_LEAVES_DISTANCE - 1, 
		MAX_LEAVES_DISTANCE - 2,
		MAX_LEAVES_DISTANCE - 2
	};

	constexpr std::array<glm::vec2, 4> GRASS_TEXT_COORDS =
	{	
		glm::vec2(0.0f, (128.0f - 16.0f) / 128.0f),
		glm::vec2(16.0f / 128.0f, (128.0f - 16.0f) / 128.0f),
		glm::vec2(16.0f / 128.0f, 1.0f),
		glm::vec2(0.0f, 1.0f)
	};

	constexpr std::array<glm::vec2, 4> GRASS_SIDE_TEXT_COORDS =
	{
		glm::vec2(16.0f / 128.0f, (128.0f - 16.0f) / 128.0f),
		glm::vec2(32.0f / 128.0f, (128.0f - 16.0f) / 128.0f),
		glm::vec2(32.0f / 128.0f, 1.0f),
		glm::vec2(16.0f / 128.0f, 1.0f)
	};

	constexpr std::array<glm::vec2, 4> SAND_TEXT_COORDS =
	{
		glm::vec2(32.0f / 128.0f, (128.0f - 16.0f) / 128.0f),
		glm::vec2(48.0f / 128.0f, (128.0f - 16.0f) / 128.0f),
		glm::vec2(48.0f / 128.0f, 1.0f),
		glm::vec2(32.0f / 128.0f, 1.0f)
	};

	constexpr std::array<glm::vec2, 4> STONE_TEXT_COORDS =
	{
		glm::vec2(48.0f / 128.0f, (128.0f - 16.0f) / 128.0f),
		glm::vec2(64.0f / 128.0f, (128.0f - 16.0f) / 128.0f),
		glm::vec2(64.0f / 128.0f, 1.0f),
		glm::vec2(48.0f / 128.0f, 1.0f)
	};

	constexpr std::array<glm::vec2, 4> WATER_TEXT_COORDS =
	{
		glm::vec2(64.0f / 128.0f, (128.0f - 16.0f) / 128.0f),
		glm::vec2(80.0f / 128.0f, (128.0f - 16.0f) / 128.0f),
		glm::vec2(80.0f / 128.0f, 1.0f),
		glm::vec2(64.0f / 128.0f, 1.0f)
	};

	constexpr std::array<glm::vec2, 4> TREESTUMP_TEXT_COORDS =
	{
		glm::vec2(80.0f / 128.0f, (128.0f - 16.0f) / 128.0f),
		glm::vec2(96.0f / 128.0f, (128.0f - 16.0f) / 128.0f),
		glm::vec2(96.0f / 128.0f, 1.0f),
		glm::vec2(80.0f / 128.0f, 1.0f)
	};

	constexpr std::array<glm::vec2, 4> LEAVES_TEXT_COORDS =
	{
		glm::vec2(96.0f / 128.0f, (128.0f - 16.0f) / 128.0f),
		glm::vec2(112.0f / 128.0f, (128.0f - 16.0f) / 128.0f),
		glm::vec2(112.0f / 128.0f, 1.0f),
		glm::vec2(96.0f / 128.0f, 1.0f)
	};

	constexpr std::array<glm::vec2, 4> CACTUS_TEXT_COORDS =
	{
		glm::vec2(112.0f / 128.0f, (128.0f - 16.0f) / 128.0f),
		glm::vec2(128.0f / 128.0f, (128.0f - 16.0f) / 128.0f),
		glm::vec2(128.0f / 128.0f, 1.0f),
		glm::vec2(112.0f / 128.0f, 1.0f)
	};

	inline void getTextCoords(std::vector<glm::vec2>& textCoords, eCubeSide cubeSide, eCubeType cubeType)
	{
		switch (cubeType)
		{
		case eCubeType::Stone:
			textCoords.insert(textCoords.end(), Utilities::STONE_TEXT_COORDS.begin(), Utilities::STONE_TEXT_COORDS.end());
			break;
		case eCubeType::Sand:
			textCoords.insert(textCoords.end(), Utilities::SAND_TEXT_COORDS.begin(), Utilities::SAND_TEXT_COORDS.end());
			break;
		case eCubeType::Grass:
			switch (cubeSide)
			{
			case eCubeSide::Back:
			case eCubeSide::Front:
			case eCubeSide::Left:
			case eCubeSide::Right:
				textCoords.insert(textCoords.end(), Utilities::GRASS_SIDE_TEXT_COORDS.begin(), Utilities::GRASS_SIDE_TEXT_COORDS.end());
				break;
			case eCubeSide::Top:
				textCoords.insert(textCoords.end(), Utilities::GRASS_TEXT_COORDS.begin(), Utilities::GRASS_TEXT_COORDS.end());
			}
			break;
		case eCubeType::TreeStump:
			textCoords.insert(textCoords.end(), Utilities::TREESTUMP_TEXT_COORDS.begin(), Utilities::TREESTUMP_TEXT_COORDS.end());
			break;
		case eCubeType::Leaves:
			textCoords.insert(textCoords.end(), Utilities::LEAVES_TEXT_COORDS.begin(), Utilities::LEAVES_TEXT_COORDS.end());
			break;
		case eCubeType::Cactus:
			textCoords.insert(textCoords.end(), Utilities::CACTUS_TEXT_COORDS.begin(), Utilities::CACTUS_TEXT_COORDS.end());
			break;
		case eCubeType::Water :
			textCoords.insert(textCoords.end(), Utilities::WATER_TEXT_COORDS.begin(), Utilities::WATER_TEXT_COORDS.end());
			break;
		}
	}

	//
	//Extern
	//

	constexpr std::array<glm::ivec3, 4> CUBE_FACE_FRONT = { glm::ivec3(0, 0, 1), glm::ivec3(1, 0, 1), glm::ivec3(1, 1, 1), glm::ivec3(0, 1, 1) };
	constexpr std::array<glm::ivec3, 4> CUBE_FACE_BACK = { glm::ivec3(1, 0, 0), glm::ivec3(0, 0, 0), glm::ivec3(0, 1, 0), glm::ivec3(1, 1, 0) };

	constexpr std::array<glm::ivec3, 4> CUBE_FACE_LEFT = { glm::ivec3(0, 0, 0), glm::ivec3(0, 0, 1), glm::ivec3(0, 1, 1), glm::ivec3(0, 1, 0) };
	constexpr std::array<glm::ivec3, 4> CUBE_FACE_RIGHT = { glm::ivec3(1, 0, 1), glm::ivec3(1, 0, 0), glm::ivec3(1, 1, 0), glm::ivec3(1, 1, 1) };

	constexpr std::array<glm::ivec3, 4> CUBE_FACE_TOP = { glm::ivec3(0, 1, 1), glm::ivec3(1, 1, 1), glm::ivec3(1, 1, 0), glm::ivec3(0, 1, 0) }; 
	constexpr std::array<glm::ivec3, 4> CUBE_FACE_BOTTOM = { glm::ivec3(0, 0, 0), glm::ivec3(1, 0, 0), glm::ivec3(1, 0, 1), glm::ivec3(0, 0, 1) }; 

	inline glm::ivec3 getNeighbouringChunkPosition(const glm::ivec3& chunkStartingPosition, eDirection direction)
	{
		switch (direction)
		{
		case eDirection::Left :
			return glm::ivec3(chunkStartingPosition.x - Utilities::CHUNK_WIDTH, 0, chunkStartingPosition.z);

		case eDirection::Right:
			return glm::ivec3(chunkStartingPosition.x + Utilities::CHUNK_WIDTH, 0, chunkStartingPosition.z);

		case eDirection::Forward :
			return glm::ivec3(chunkStartingPosition.x, 0, chunkStartingPosition.z + Utilities::CHUNK_DEPTH);
		
		case eDirection::Back:
			return glm::ivec3(chunkStartingPosition.x, 0, chunkStartingPosition.z - Utilities::CHUNK_DEPTH);

		default:
			assert(false);
		}
	}

	inline void getClosestMiddlePosition(glm::ivec3& position)
	{
		if (position.x % (CHUNK_WIDTH / 2) < 0)
		{
			position.x += std::abs(position.x % CHUNK_WIDTH / 2);
			position.x -= CHUNK_WIDTH / 2;
		}
		else if (position.x % (CHUNK_WIDTH / 2) > 0)
		{
			position.x -= std::abs(position.x % CHUNK_WIDTH / 2);
			position.x += CHUNK_WIDTH / 2;
		}
		if (position.z % (CHUNK_DEPTH / 2) < 0)
		{
			position.z += std::abs(position.z % CHUNK_DEPTH / 2);
			position.z -= CHUNK_DEPTH / 2;
		}
		else if (position.z % (CHUNK_DEPTH / 2) > 0)
		{
			position.z -= std::abs(position.z % CHUNK_DEPTH / 2);
			position.z += CHUNK_DEPTH / 2;
		}
	}

	inline void getClosestChunkStartingPosition(glm::ivec3& position)
	{
		if (position.x % CHUNK_WIDTH < 0)
		{
			position.x += std::abs(position.x % CHUNK_WIDTH);
			position.x -= CHUNK_WIDTH;
		}
		else if (position.x % CHUNK_WIDTH > 0)
		{
			position.x -= std::abs(position.x % CHUNK_WIDTH);
		}
		if (position.z % CHUNK_DEPTH < 0)
		{
			position.z += std::abs(position.z % CHUNK_DEPTH);
			position.z -= CHUNK_DEPTH;
		}
		else if (position.z % CHUNK_DEPTH > 0)
		{
			position.z -= std::abs(position.z % CHUNK_DEPTH);
		}
	}

	inline int converTo1D(const glm::ivec3& position) 
	{
		return (position.z * Utilities::CHUNK_WIDTH * Utilities::CHUNK_HEIGHT) + (position.y * Utilities::CHUNK_WIDTH) + position.x;
	}

	inline glm::ivec3 convertTo3D(int idx) 
	{
		glm::ivec3 position;
		position.z = idx / (Utilities::CHUNK_WIDTH * Utilities::CHUNK_HEIGHT);
		idx -= (position.z * Utilities::CHUNK_WIDTH * Utilities::CHUNK_HEIGHT);
		position.y = idx / Utilities::CHUNK_WIDTH;
		position.x = idx % Utilities::CHUNK_WIDTH;
		return position;
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

	const std::array<unsigned int, 6> CUBE_FACE_INDICIES =
	{
		0, 1, 2,
		2, 3, 0
	};
}