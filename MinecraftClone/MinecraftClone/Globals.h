#pragma once

#include "glm/glm.hpp"
#include "CubeType.h"
#include <string>
#include <array>
#include <random>

enum class eInventoryIndex
{
	One = 0,
	Two,
	Three, 
	Four, 
	Five, 
	Six, 
	Seven, 
	Eight,
	Max = Eight
};

enum class eTerrainTextureLayer
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

enum class eGuiTextureLayer
{
	InventoryBar,
	Selected
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
	Forward,
	Back,
	Max = Back
};

//https://stackoverflow.com/questions/4422507/superiority-of-unnamed-namespace-over-static

namespace Globals
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
	constexpr int MAX_TREE_PER_CHUNK = 3;
	constexpr int MAX_CACTUS_PER_CHUNK = 1;
	
	constexpr int MAX_SHRUB_PER_CHUNK = 3;
	constexpr int MAX_TALL_GRASS_PER_CHUNK = 20;
	constexpr int MAX_PLANT_SPAWN_ATTEMPTS = 20;
	constexpr int MAX_CACTUS_SPAWN_ATTEMPTS = 20;
	constexpr int MAX_TREE_SPAWN_ATTEMPTS = 5;
	constexpr int CHANCE_TREE_SPAWN_IN_CHUNK = 50;

	constexpr unsigned int INVALID_OPENGL_ID = 0;

	constexpr float BIOME_LACUNARITY = 5.0f;
	constexpr float BIOME_PERSISTENCE = 0.5f;
	constexpr int BIOME_OCTAVES = 12;

	constexpr float TERRAIN_LACUNARITY = 8.0f;
	constexpr float TERRAIN_PERSISTENCE = 2.5f;
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

	const CubeTypeComparison NON_COLLIDABLE_CUBE_TYPES =
	{
		{eCubeType::Water,
		eCubeType::TallGrass,
		eCubeType::Shrub}
	};

	inline int getRandomNumber(int min, int max)
	{
		static std::random_device rd;  //Will be used to obtain a seed for the random number engine
		static std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
		std::uniform_int_distribution<> dis(min, max);

		return dis(gen);
	}

	inline glm::ivec3 getClosestMiddlePosition(const glm::ivec3& position)
	{
		glm::ivec3 middlePosition = position;
		if (position.x % (Globals::CHUNK_WIDTH / 2) < 0)
		{
			middlePosition.x += std::abs(position.x % Globals::CHUNK_WIDTH / 2);
			middlePosition.x -= Globals::CHUNK_WIDTH / 2;
		}
		else if (position.x % (Globals::CHUNK_WIDTH / 2) > 0)
		{
			middlePosition.x -= std::abs(position.x % Globals::CHUNK_WIDTH / 2);
			middlePosition.x += Globals::CHUNK_WIDTH / 2;
		}
		if (position.z % (Globals::CHUNK_DEPTH / 2) < 0)
		{
			middlePosition.z += std::abs(position.z % Globals::CHUNK_DEPTH / 2);
			middlePosition.z -= Globals::CHUNK_DEPTH / 2;
		}
		else if (position.z % (Globals::CHUNK_DEPTH / 2) > 0)
		{
			middlePosition.z -= std::abs(position.z % Globals::CHUNK_DEPTH / 2);
			middlePosition.z += Globals::CHUNK_DEPTH / 2;
		}

		return { middlePosition.x, 0, middlePosition.z };
	}

	constexpr int CUBE_SIZE = 1;
	//384 dev distance
	constexpr int VISIBILITY_DISTANCE = 384;
	constexpr int MAP_SIZE = 8000;
	const std::string TEXTURE_DIRECTORY = "Textures/";
	const std::string FONTS_DIRECTORY = "Fonts/";
	constexpr glm::ivec3 PLAYER_STARTING_POSITION = { 0.0f, 0.0f, 0.0f };
}