#pragma once

#include "glm/glm.hpp"
#include "CubeID.h"
#include <string>

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
	
	constexpr int MAX_SHRUB_PER_CHUNK = 3;
	constexpr int MAX_TALL_GRASS_PER_CHUNK = 20;
	constexpr int MAX_PLANT_SPAWN_ATTEMPTS = 20;
	constexpr int MAX_CACTUS_SPAWN_ATTEMPTS = 20;
	constexpr int MAX_TREE_SPAWN_ATTEMPTS = 5;
	constexpr int CHANCE_TREE_SPAWN_IN_CHUNK = 50;

	constexpr unsigned int INVALID_OPENGL_ID = 0;

	constexpr float MOISTURE_LACUNARITY = 5.0f;
	constexpr float MOISTURE_PERSISTENCE = 5.0f;
	constexpr int MOISTURE_OCTAVES = 3;

	constexpr float DESERT_LACUNARITY = 2.5f;
	constexpr float DESERT_PERSISTENCE = 10.0f;
	constexpr float DESERT_REDISTRIBUTION = 1.0f;

	constexpr float PLAINS_LACUNARITY = 2.0f;
	constexpr float PLAINS_PERSISTENCE = 10.5f;
	constexpr float PLAINS_REDISTRIBUTION = 1.0f;

	constexpr int TERRAIN_OCTAVES = 8;

	constexpr int VISIBILITY_DISTANCE = 832;
	constexpr int MAP_SIZE = 8000;
	const glm::vec3 PLAYER_STARTING_POSITION(0.0f, 100.f, 0.0f);
	const std::string TEXTURE_DIRECTORY = "Textures/";
	const std::string FONTS_DIRECTORY = "Fonts/";
}