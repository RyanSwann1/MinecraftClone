#pragma once

#include "glm/glm.hpp"
#include "CubeID.h"
#include <array>
#include <vector>
#include <random>

enum class eTextureLayer
{
	Grass = 0,
	GrassSide,
	Sand,
	Stone,
	Water,
	Log,
	LogTop,
	Leaves,
	Cactus,
	CactusTop,
	Shrub,
	TallGrass,
	Error,
	Max = Error
};

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
	constexpr int MIN_TREE_HEIGHT = 6;
	constexpr int MAX_TREE_HEIGHT = 9;
	constexpr int CACTUS_MIN_HEIGHT = 1;
	constexpr int CACTUS_MAX_HEIGHT = 4;
	constexpr int MAX_TREE_PER_CHUNK = 1;
	constexpr int MAX_CACTUS_PER_CHUNK = 1;
	constexpr int MAX_LEAVES_DISTANCE = 3;
	constexpr int MAX_SHRUB_PER_CHUNK = 3;
	constexpr int MAX_TALL_GRASS_PER_CHUNK = 20;
	constexpr int MAX_PLANT_SPAWN_ATTEMPTS = 20;
	constexpr int MAX_CACTUS_SPAWN_ATTEMPTS = 20;
	constexpr int MAX_TREE_SPAWN_ATTEMPTS = 5;
	constexpr int CHANCE_TREE_SPAWN_IN_CHUNK = 50;

	constexpr unsigned int INVALID_OPENGL_ID = 0;

	constexpr float MOISTURE_LACUNARITY = 5.0f;
	constexpr float MOISTURE_PERSISTENCE = 0.5f;
	constexpr int MOISTURE_OCTAVES = 3;

	constexpr float TERRAIN_LACUNARITY = 15.f;
	constexpr float TERRAIN_PERSISTENCE = 0.5f;
	constexpr int TERRAIN_OCTAVES = 8;

	constexpr int VISIBILITY_DISTANCE = 1024;
	constexpr int MAP_SIZE = 8000;
	const glm::vec3 PLAYER_STARTING_POSITION(0.0f, 100.f, 0.0f);
	const std::string TEXTURE_DIRECTORY = "Textures/";

	constexpr std::array<int, 6> LEAVES_DISTANCES =
	{
		MAX_LEAVES_DISTANCE, 
		MAX_LEAVES_DISTANCE, 
		MAX_LEAVES_DISTANCE - 1, 
		MAX_LEAVES_DISTANCE - 1, 
		MAX_LEAVES_DISTANCE - 2,
		MAX_LEAVES_DISTANCE - 2
	};

	//
	//Extern
	//
	
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
}