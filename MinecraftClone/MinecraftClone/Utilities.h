#pragma once

#include "glm/glm.hpp"
#include "CubeType.h"
#include <string>
#include <array>

enum class eTextureLayer
{
	Grass = 0,
	GrassSide,
	Dirt,
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
	constexpr int CHUNK_HEIGHT = 320;
	constexpr int CHUNK_DEPTH = 32;
	constexpr int CHUNK_VOLUME = CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_DEPTH;
	constexpr int MAX_SHADOW_HEIGHT = 8;

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

	constexpr float BIOME_LACUNARITY = 2.5f;
	constexpr float BIOME_PERSISTENCE = 1.0f;
	constexpr int BIOME_OCTAVES = 12;

	constexpr float DESERT_LACUNARITY = 30.0f;
	constexpr float DESERT_PERSISTENCE = 10.0f;
	constexpr int DESERT_OCTAVES = 8;
	constexpr float DESERT_REDISTRIBUTION = 1.5f;
	constexpr int DESERT_MAX_HEIGHT = 50;

	constexpr float PLAINS_LACUNARITY = 25.0f;
	constexpr float PLAINS_PERSISTENCE = 5.0f;
	constexpr int PLAINS_OCTAVES = 8;
	constexpr float PLAINS_REDISTRIBUTION = 1.05f;
	constexpr int PLAINS_MAX_HEIGHT = 50;

	constexpr float MOUNTAINS_LACUNARITY = 25.0f;
	constexpr float MOUNTAINS_PERSISTENCE = 5.0f;
	constexpr int MOUNTAINS_OCTAVES = 16;
	constexpr float MOUNTAINS_REDISTRIBUTION = 15.0f;
	constexpr int MOUNTAINS_MAX_HEIGHT = 288;

	constexpr float ISLANDS_LACUNARITY = 25.0f;
	constexpr float ISLANDS_PERSISTENCE = 5.0f;
	constexpr int ISLANDS_OCTAVES = 16;
	constexpr float ISLANDS_REDISTRIBUTION = 2.25f;
	constexpr int ISLANDS_MAX_HEIGHT = 35;

	constexpr float DESERT_MOUNTAINS_LACUNARITY = 1.0f;
	constexpr float DESERT_MOUNTAINS_PERSISTENCE = 5.0f;
	constexpr int DESERT_MOUNTAINS_OCTAVES = 8;
	constexpr float DESERT_MOUNTAINS_REDISTRIBUTION = 2.8f;
	constexpr int DESERT_MOUNTAINS_MAX_HEIGHT = 5;

	constexpr float TERRAIN_LACUNARITY = 35.0f;
	constexpr float TERRAIN_PERSISTENCE = 11.5f;

	constexpr int TERRAIN_OCTAVES = 8;

	const CubeTypeComparison TRANSPARENT_CUBE_TYPES =
	{
		{eCubeType::Water,
		eCubeType::Leaves,
		eCubeType::Shrub,
		eCubeType::TallGrass}
	};

	const CubeTypeComparison OPAQUE_CUBE_TYPES =
	{
		{eCubeType::Grass,
		eCubeType::Dirt,
		eCubeType::Sand,
		eCubeType::Stone,
		eCubeType::Log,
		eCubeType::LogTop,
		eCubeType::Cactus,
		eCubeType::CactusTop}
	};

	//constexpr int VISIBILITY_DISTANCE = 128;
	constexpr int VISIBILITY_DISTANCE = 1024;

	constexpr int MAP_SIZE = 8000;
	const glm::vec3 PLAYER_STARTING_POSITION(0.0f, 100.f, 0.0f);
	const std::string TEXTURE_DIRECTORY = "Textures/";
	const std::string FONTS_DIRECTORY = "Fonts/";
}