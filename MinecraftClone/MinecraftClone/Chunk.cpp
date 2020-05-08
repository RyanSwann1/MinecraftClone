#include "Chunk.h"
#include "VertexBuffer.h"
#include "VertexBuffer.h"
#include "glm/gtc/noise.hpp"
#include <iostream>
#include <algorithm>
#include <limits>
#include <random>

//OLD VALUES

//MAP_SIZE == 8000;
//double ex = (x) / (Utilities::MAP_SIZE * 1.0f) - 0.5f;
//double ey = (z) / (Utilities::MAP_SIZE * 1.0f) - 0.5f;
//
//float elevation = std::abs(1 * glm::perlin(glm::vec2(5.0f * ex, 5.0f * ey)));
//elevation += std::abs(0.5 * glm::perlin(glm::vec2(ex * 15.0f, ey * 15.0f)));
//elevation += std::abs(0.25 * glm::perlin(glm::vec2(ex * 30.0f, ey * 30.0f)));
//
//elevation = glm::pow(elevation, 2.5f);
//elevation = elevation * (float)Utilities::CHUNK_HEIGHT;
//elevation = Utilities::clampTo(elevation, 0.0f, (float)Utilities::CHUNK_HEIGHT - 1.0f);
//
//double mx = (x) / (Utilities::MAP_SIZE * 1.0f) - 0.5f;
//double my = (z) / (Utilities::MAP_SIZE * 1.0f) - 0.5f;
//float moisture = std::abs(1 * glm::perlin(glm::vec2(15.0f * mx, 15.0f * my)));

namespace
{
	constexpr int LERP_DISTANCE = 10;

	constexpr float BIOME_LACUNARITY = 2.5f;
	constexpr float BIOME_PERSISTENCE = 1.0f;
	constexpr int BIOME_OCTAVES = 12;

	constexpr float DESERT_LACUNARITY = 5.0f;
	constexpr float DESERT_PERSISTENCE = 1.0f;
	constexpr int DESERT_OCTAVES = 8;
	constexpr float DESERT_REDISTRIBUTION = 1.5f;
	constexpr int DESERT_MAX_HEIGHT = 50;

	constexpr float PLAINS_LACUNARITY = 5.0f;
	constexpr float PLAINS_PERSISTENCE = 3.0f;
	constexpr int PLAINS_OCTAVES = 8;
	constexpr float PLAINS_REDISTRIBUTION = 1.05f;
	constexpr int PLAINS_MAX_HEIGHT = 75;

	constexpr float MOUNTAINS_LACUNARITY = 50.0f;
	constexpr float MOUNTAINS_PERSISTENCE = 50.0f;
	constexpr int MOUNTAINS_OCTAVES = 16;
	constexpr float MOUNTAINS_REDISTRIBUTION = 1.8f;
	constexpr int MOUNTAINS_MAX_HEIGHT = 168;

	constexpr float ISLANDS_LACUNARITY = 25.0f;
	constexpr float ISLANDS_PERSISTENCE = 50.0f;
	constexpr int ISLANDS_OCTAVES = 16;
	constexpr float ISLANDS_REDISTRIBUTION = 2.25f;
	constexpr int ISLANDS_MAX_HEIGHT = 45;

	constexpr float TERRAIN_LACUNARITY = 35.0f;
	constexpr float TERRAIN_PERSISTENCE = 11.5f;

	constexpr int TERRAIN_OCTAVES = 8;

	constexpr int MAX_LEAVES_DISTANCE = 3;
	constexpr std::array<int, 6> LEAVES_DISTANCES =
	{
		MAX_LEAVES_DISTANCE,
		MAX_LEAVES_DISTANCE,
		MAX_LEAVES_DISTANCE - 1,
		MAX_LEAVES_DISTANCE - 1,
		MAX_LEAVES_DISTANCE - 2,
		MAX_LEAVES_DISTANCE - 2
	};

	int converTo1D(const glm::ivec3& position)
	{
		return (position.z * Utilities::CHUNK_WIDTH * Utilities::CHUNK_HEIGHT) + (position.y * Utilities::CHUNK_WIDTH) + position.x;
	}

	glm::ivec3 convertTo3D(int idx)
	{
		glm::ivec3 position;
		position.z = idx / (Utilities::CHUNK_WIDTH * Utilities::CHUNK_HEIGHT);
		idx -= (position.z * Utilities::CHUNK_WIDTH * Utilities::CHUNK_HEIGHT);
		position.y = idx / Utilities::CHUNK_WIDTH;
		position.x = idx % Utilities::CHUNK_WIDTH;

		return position;
	}

	int getRandomNumber(int min, int max)
	{
		static std::random_device rd;  //Will be used to obtain a seed for the random number engine
		static std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
		std::uniform_int_distribution<> dis(min, max);

		return dis(gen);
	}

	float bilinearInterpolation(float bottomLeft, float topLeft, float bottomRight, float topRight,
		float xMin, float xMax,
		float zMin, float zMax,
		float x, float z)
	{
		float   width = xMax - xMin,
			height = zMax - zMin,

			xDistanceToMaxValue = xMax - x,
			zDistanceToMaxValue = zMax - z,

			xDistanceToMinValue = x - xMin,
			zDistanceToMinValue = z - zMin;

		return 1.0f / (width * height) *
			(
				bottomLeft * xDistanceToMaxValue * zDistanceToMaxValue +
				bottomRight * xDistanceToMinValue * zDistanceToMaxValue +
				topLeft * xDistanceToMaxValue * zDistanceToMinValue +
				topRight * xDistanceToMinValue * zDistanceToMinValue
				);
	}

	int lerp(int pointA, int pointB, float factor)
	{
		if (factor > 1.f)
		{
			factor = 1.f;
		}
		else if (factor < 0.f)
		{
			factor = 0.f;
		}

		return pointA + (pointB - pointA) * factor;
	}
}

Chunk::Chunk()
	: m_startingPosition(),
	m_endingPosition(),
	m_chunk(),
	m_AABB()
{}

Chunk::Chunk(const glm::ivec3& startingPosition)
	: m_startingPosition(startingPosition),
	m_endingPosition(startingPosition.x + Utilities::CHUNK_WIDTH, 
		startingPosition.y + Utilities::CHUNK_HEIGHT, 
		startingPosition.z + Utilities::CHUNK_DEPTH),
	m_chunk(),
	m_AABB(glm::ivec2(m_startingPosition.x, m_startingPosition.z) +
		glm::ivec2(Utilities::CHUNK_WIDTH / 2, Utilities::CHUNK_DEPTH / 2), 16)
{
	regen(m_startingPosition);
}

Chunk::Chunk(Chunk&& orig) noexcept
	: m_startingPosition(orig.m_startingPosition),
	m_endingPosition(orig.m_endingPosition),
	m_chunk(std::move(orig.m_chunk)),
	m_AABB(orig.m_AABB)
{}

Chunk& Chunk::operator=(Chunk&& orig) noexcept
{
	m_startingPosition = orig.m_startingPosition;
	m_endingPosition = orig.m_endingPosition;
	m_chunk = std::move(orig.m_chunk);
	m_AABB = orig.m_AABB;

	return *this;
}

bool Chunk::isCubeBelowCovering(const glm::ivec3& position) const
{
	if (!isPositionInBounds(glm::ivec3(position.x, position.y + 1, position.z)))
	{
		return false;
	}

	glm::ivec3 startingPositionOnGrid(position.x - m_startingPosition.x, position.y - m_startingPosition.y, position.z - m_startingPosition.z);
	if (!isCubeAtLocalPosition(startingPositionOnGrid, eCubeType::Invalid) &&
		isCubeAtLocalPosition(glm::ivec3(startingPositionOnGrid.x, startingPositionOnGrid.y + 1, startingPositionOnGrid.z), eCubeType::Invalid) || 
		isCubeAtLocalPosition(glm::ivec3(startingPositionOnGrid.x, startingPositionOnGrid.y + 1, startingPositionOnGrid.z), eCubeType::TallGrass))
	{
		for (int y = Utilities::CHUNK_HEIGHT - 1; y > startingPositionOnGrid.y + 2; --y)
		{
			glm::ivec3 positionOnGrid = glm::ivec3(position.x - m_startingPosition.x, y, position.z - m_startingPosition.z);
			if (!isCubeAtLocalPosition(positionOnGrid, eCubeType::Invalid))
			{
				return true;
			}
		}
	}

	return false;
}

const Rectangle& Chunk::getAABB() const
{
	return m_AABB;
}

bool Chunk::isPositionInBounds(const glm::ivec3& position) const
{
	return (position.x >= m_startingPosition.x &&
		position.y >= m_startingPosition.y &&
		position.z >= m_startingPosition.z &&
		position.x <= m_endingPosition.x - 1 &&
		position.y <= m_endingPosition.y - 1 &&
		position.z <= m_endingPosition.z - 1);
}

const glm::ivec3& Chunk::getStartingPosition() const
{
	return m_startingPosition;
}

const glm::ivec3& Chunk::getEndingPosition() const
{
	return m_endingPosition;
}

char Chunk::getCubeDetailsWithoutBoundsCheck(const glm::ivec3& position) const
{
	glm::ivec3 positionOnGrid(position.x - m_startingPosition.x, position.y - m_startingPosition.y, position.z - m_startingPosition.z);
	return m_chunk[converTo1D(positionOnGrid)];
}

void Chunk::changeCubeAtLocalPosition(const glm::ivec3& position, eCubeType cubeType)
{
	assert(isPositionInLocalBounds(position));
	m_chunk[converTo1D(position)] = static_cast<char>(cubeType);
}

void Chunk::reset()
{
	m_startingPosition = glm::ivec3();
	m_endingPosition = glm::ivec3();

	m_AABB = Rectangle();
}

void Chunk::reuse(const glm::ivec3& startingPosition)
{	
	memset(&m_chunk, static_cast<char>(eCubeType::Invalid), m_chunk.size());
	m_heightMap = std::array<std::array<HeightMapNode, 32>, 32>();

	m_startingPosition = startingPosition;
	m_endingPosition = glm::ivec3(startingPosition.x + Utilities::CHUNK_WIDTH, startingPosition.y + Utilities::CHUNK_HEIGHT,
		startingPosition.z + Utilities::CHUNK_DEPTH);
	m_AABB.reset(glm::ivec2(m_startingPosition.x, m_startingPosition.z) +
		glm::ivec2(Utilities::CHUNK_WIDTH / 2, Utilities::CHUNK_DEPTH / 2), 16);

	regen(m_startingPosition);	
}

//Scale
//Octaves
///Lacunraity
//Persistance
//https://www.reddit.com/r/proceduralgeneration/comments/byju4s/minecraft_style_terrain_gen_question_how_to/
//https://www.reddit.com/r/proceduralgeneration/comments/dkdfq0/different_generation_for_biomes/

//https://rtouti.github.io/graphics/perlin-noise-algorithm
//http://www.6by9.net/simplex-noise-for-c-and-python/
//https://medium.com/@yvanscher/playing-with-perlin-noise-generating-realistic-archipelagos-b59f004d8401

//Lacunarity = Controls the increase in frequency in octaves
//Persistence = Controls decrease in amplitude of octaves

//Amplitude - 'y' Axis
//Frequency - 'x' Axis

//https://www.reddit.com/r/proceduralgeneration/comments/4i9a08/terrain_generation_of_a_game_i_am_working_on/
//http://pcgbook.com/wp-content/uploads/chapter04.pdf
//https://www.reddit.com/r/proceduralgeneration/comments/drc96v/getting_started_in_proceduralgeneration/
//https://notch.tumblr.com/post/3746989361/terrain-generation-part-1

//https://www.reddit.com/r/proceduralgeneration/comments/4i9a08/terrain_generation_of_a_game_i_am_working_on/
//http://pcgbook.com/wp-content/uploads/chapter04.pdf
//https://www.reddit.com/r/proceduralgeneration/comments/drc96v/getting_started_in_proceduralgeneration/
//https://notch.tumblr.com/post/3746989361/terrain-generation-part-1

//https://www.reddit.com/r/VoxelGameDev/comments/c0fcsi/giving_terrain_oomph_ive_been_trying_to_go_for/

//http://accidentalnoise.sourceforge.net/minecraftworlds.html

void Chunk::regen(const glm::ivec3& startingPosition)
{
	//https://www.reddit.com/r/proceduralgeneration/search?q=biome%20transition&restrict_sr=1

	constructHeightMap({ startingPosition.x, startingPosition.z });
	int lerpFactor = 0;

	for (int z = startingPosition.z; z < startingPosition.z + Utilities::CHUNK_DEPTH; ++z)
	for (int x = startingPosition.x; x < startingPosition.x + Utilities::CHUNK_WIDTH; ++x)
	{
		eCubeType cubeType;

		switch (getBiomeType(x, z))
		{
		case eBiomeType::Plains :
		{
			//int elevation = getElevationAtPosition(x, z, PLAINS_LACUNARITY, PLAINS_PERSISTENCE, 
			//	PLAINS_OCTAVES, PLAINS_REDISTRIBUTION, PLAINS_MAX_HEIGHT);
			int elevation = m_heightMap[z - startingPosition.z][x - startingPosition.x].height;
			glm::ivec3 positionOnGrid(x - startingPosition.x, elevation, z - startingPosition.z);
			for (int y = elevation; y >= 0; --y)
			{
				if (y <= Utilities::STONE_MAX_HEIGHT)
				{
					cubeType = eCubeType::Stone;
				}
				else
				{
					cubeType = eCubeType::Grass;
				}

				changeCubeAtLocalPosition({ positionOnGrid.x, y, positionOnGrid.z }, cubeType);
			}
		}
		break;
		case eBiomeType::Desert:
		{
			//int elevation = getElevationAtPosition(x, z, DESERT_LACUNARITY, DESERT_PERSISTENCE,
			//	DESERT_OCTAVES, DESERT_REDISTRIBUTION, DESERT_MAX_HEIGHT);
			int elevation = m_heightMap[z - startingPosition.z][x - startingPosition.x].height;
			glm::ivec3 positionOnGrid(x - startingPosition.x, elevation, z - startingPosition.z);
			for (int y = elevation; y >= 0; --y)
			{
				if (y <= Utilities::STONE_MAX_HEIGHT)
				{
					cubeType = eCubeType::Stone;
				}
				else
				{
					cubeType = eCubeType::Sand;
				}

				changeCubeAtLocalPosition({ positionOnGrid.x, y, positionOnGrid.z }, cubeType);
			}
		}
		break;
		case eBiomeType::Mountains :
		{
			int elevation = getElevationAtPosition(x, z, MOUNTAINS_LACUNARITY, MOUNTAINS_PERSISTENCE,
				MOUNTAINS_OCTAVES, MOUNTAINS_REDISTRIBUTION, MOUNTAINS_MAX_HEIGHT);
			glm::ivec3 positionOnGrid(x - startingPosition.x, elevation, z - startingPosition.z);
			for (int y = elevation; y >= 0; --y)
			{
				cubeType = eCubeType::Stone;

				changeCubeAtLocalPosition({ positionOnGrid.x, y, positionOnGrid.z }, cubeType);
			}
		}
		break;
		case eBiomeType::Islands:
		{
			int elevation = getElevationAtPosition(x, z, ISLANDS_LACUNARITY, ISLANDS_PERSISTENCE,
				ISLANDS_OCTAVES, ISLANDS_REDISTRIBUTION, ISLANDS_MAX_HEIGHT);
			glm::ivec3 positionOnGrid(x - startingPosition.x, elevation, z - startingPosition.z);
			for (int y = elevation; y >= 0; --y)
			{
				if (y <= Utilities::STONE_MAX_HEIGHT)
				{
					cubeType = eCubeType::Stone;
				}
				else
				{
					cubeType = eCubeType::Grass;
				}

				changeCubeAtLocalPosition({ positionOnGrid.x, y, positionOnGrid.z }, cubeType);
			}
		}
		break;
		default:
			assert(false);
		}
		
	}

	//Spawn Decoration
	spawnWater();
	spawnTrees();
	spawnCactus();
	spawnPlant(Utilities::MAX_SHRUB_PER_CHUNK, eCubeType::Sand, eCubeType::Shrub);
	spawnPlant(Utilities::MAX_TALL_GRASS_PER_CHUNK, eCubeType::Grass, eCubeType::TallGrass);
}

void Chunk::spawnWater()
{
	for (int z = 0; z < Utilities::CHUNK_DEPTH; ++z)
	{
		for (int x = 0; x < Utilities::CHUNK_WIDTH; ++x)
		{
			glm::ivec3 position(x, Utilities::WATER_MAX_HEIGHT, z);
			if (isCubeAtLocalPosition(position, eCubeType::Invalid))
			{
				changeCubeAtLocalPosition(position, eCubeType::Water);
			}
		}
	}
}

void Chunk::spawnTrees()
{
	if (getRandomNumber(0, 100) < Utilities::CHANCE_TREE_SPAWN_IN_CHUNK)
	{
		return;
	}
	
	int attemptsCount = 0;
	int spawnCount = 0;
	while (attemptsCount < Utilities::MAX_TREE_SPAWN_ATTEMPTS && spawnCount < Utilities::MAX_TREE_PER_CHUNK)
	{
		glm::ivec3 spawnPosition;
		spawnPosition.x = getRandomNumber(MAX_LEAVES_DISTANCE, Utilities::CHUNK_WIDTH - MAX_LEAVES_DISTANCE - 1);
		spawnPosition.z = getRandomNumber(MAX_LEAVES_DISTANCE, Utilities::CHUNK_DEPTH - MAX_LEAVES_DISTANCE - 1);

		//Find Spawn Location
		for (int y = Utilities::CHUNK_HEIGHT - Utilities::MAX_TREE_HEIGHT - MAX_LEAVES_DISTANCE - 1; y >= Utilities::SAND_MAX_HEIGHT; --y)
		{
			spawnPosition.y = y;
			if (isCubeAtLocalPosition(spawnPosition, eCubeType::Grass) &&
				isCubeAtLocalPosition({ spawnPosition.x, spawnPosition.y + 1, spawnPosition.z }, eCubeType::Invalid))
			{
				int treeHeight = getRandomNumber(Utilities::MIN_TREE_HEIGHT, Utilities::MAX_TREE_HEIGHT);
				spawnLeaves(spawnPosition, treeHeight);
				spawnTreeStump(spawnPosition, treeHeight);
				++spawnCount;
				break;
			}
		}

		++attemptsCount;
	}
}

void Chunk::spawnCactus()
{
	int attemptsCount = 0;
	int spawnCount = 0;
	while (attemptsCount < Utilities::MAX_CACTUS_SPAWN_ATTEMPTS && spawnCount < Utilities::MAX_CACTUS_PER_CHUNK)
	{
		glm::ivec3 spawnPosition;
		spawnPosition.x = getRandomNumber(0, Utilities::CHUNK_WIDTH - 1);
		spawnPosition.z = getRandomNumber(0, Utilities::CHUNK_DEPTH - 1);

		//Find Spawn Location
		for (int y = Utilities::CHUNK_HEIGHT - Utilities::CACTUS_MAX_HEIGHT - 1; y >= 0; --y)
		{
			spawnPosition.y = y;
			if (isCubeAtLocalPosition(spawnPosition, eCubeType::Sand) &&
				isCubeAtLocalPosition({ spawnPosition.x, y + 1, spawnPosition.z }, eCubeType::Invalid))
			{
				//Spawn Cactus
				int cactusHeight = getRandomNumber(Utilities::CACTUS_MIN_HEIGHT, Utilities::CACTUS_MAX_HEIGHT);
				for (int i = 1; i <= cactusHeight; ++i)
				{
					if (i == cactusHeight)
					{
						changeCubeAtLocalPosition({ spawnPosition.x, spawnPosition.y + i, spawnPosition.z }, eCubeType::CactusTop);
					}
					else
					{
						changeCubeAtLocalPosition({ spawnPosition.x, spawnPosition.y + i, spawnPosition.z }, eCubeType::Cactus);
					}
				}

				++spawnCount;
				break;
			}
		}

		++attemptsCount;
	}
}

void Chunk::spawnPlant(int maxQuantity, eCubeType baseCubeType, eCubeType plantCubeType)
{
	int attemptsCount = 0;
	int spawnCount = 0;
	while (attemptsCount < Utilities::MAX_PLANT_SPAWN_ATTEMPTS && spawnCount < maxQuantity)
	{
		glm::ivec3 spawnPosition;
		spawnPosition.x = getRandomNumber(0, Utilities::CHUNK_WIDTH - 1);
		spawnPosition.z = getRandomNumber(0, Utilities::CHUNK_DEPTH - 1);

		for (int y = Utilities::CHUNK_HEIGHT - 5; y >= Utilities::WATER_MAX_HEIGHT; --y)
		{
			spawnPosition.y = y;
			if (isCubeAtLocalPosition(spawnPosition, eCubeType::Invalid) &&
				isCubeAtLocalPosition({ spawnPosition.x, y - 1, spawnPosition.z }, baseCubeType))
			{
				changeCubeAtLocalPosition(spawnPosition, plantCubeType);
				++spawnCount;
			}
		}

		++attemptsCount;
	}
}

void Chunk::spawnLeaves(const glm::ivec3& startingPosition, int treeHeight)
{
	int y = startingPosition.y + treeHeight / 2; //Starting Height
	for (int distance : LEAVES_DISTANCES)
	{
		++y;
		for (int z = startingPosition.z - distance; z <= startingPosition.z + distance; ++z)
		{
			for (int x = startingPosition.x - distance; x <= startingPosition.x + distance; ++x)
			{
				glm::ivec3 position(x, y, z);
				if (isCubeAtLocalPosition(position, eCubeType::Invalid))
				{
					changeCubeAtLocalPosition(position, eCubeType::Leaves);
				}
			}
		}
	}
}

void Chunk::spawnTreeStump(const glm::ivec3& startingPosition, int treeHeight)
{
	for (int y = startingPosition.y; y <= startingPosition.y + treeHeight / 2; ++y)
	{
		if (y == startingPosition.y + treeHeight / 2)
		{
			changeCubeAtLocalPosition({ startingPosition.x, y, startingPosition.z }, eCubeType::LogTop);
		}
		else
		{
			changeCubeAtLocalPosition({ startingPosition.x, y, startingPosition.z }, eCubeType::Log);
		}
	}
}

void Chunk::constructHeightMap(const glm::ivec2& startingPosition)
{
	for (int y = 0; y < Utilities::CHUNK_DEPTH; ++y)
	for (int x = 0; x <= Utilities::CHUNK_WIDTH; x += LERP_DISTANCE)
	{
		if (x == Utilities::CHUNK_WIDTH)
		{
			--x;
			m_heightMap[y][x].height = getElevationAtPosition({ x + startingPosition.x, y + startingPosition.y });
			break;
		}
		else
		{
			m_heightMap[y][x].height = getElevationAtPosition({ x + startingPosition.x, y + startingPosition.y });
		}
	}
	
	int pointA = 0;
	int pointB = 0;
	float lerpFactor = 0;
	for (int y = 0; y < Utilities::CHUNK_DEPTH; ++y)
	{
		for (int x = 0; x < Utilities::CHUNK_WIDTH; ++x)
		{
			int height = m_heightMap[y][x].height;
			if (height)
			{
				if (x + LERP_DISTANCE >= Utilities::CHUNK_WIDTH)
				{
					pointB = m_heightMap[y][Utilities::CHUNK_WIDTH - 1].height;
				}
				else
				{
					pointB = m_heightMap[y][x + LERP_DISTANCE].height;
				}

				pointA = height;
				lerpFactor = 0;
			}
			else
			{
				lerpFactor += 0.10f;
				m_heightMap[y][x].height = lerp(pointA, pointB, lerpFactor);
			}
		}
	}

	////Add noise to biome transitions
	//for (int y = 0; y < Utilities::CHUNK_DEPTH; ++y)
	//{
	//	for (int x = 0; x < Utilities::CHUNK_WIDTH; x += 4)
	//	{
	//		if (isPositionOnBiomeTransition({ x + startingPosition.x, y + startingPosition.y }, 4))
	//		{
	//			int height = m_heightMap[y][x].height;
	//			m_heightMap[y][x].height = getRandomNumber(height - 3, height + 3);
	//		}
	//	}
	//}

	//int bottomLeft = getElevationAtPosition(glm::ivec2(startingPosition.x, startingPosition.y));
	//int bottomRight = getElevationAtPosition(glm::ivec2(startingPosition.x + Utilities::CHUNK_WIDTH, startingPosition.y));
	//int topLeft = getElevationAtPosition(glm::ivec2(startingPosition.x, startingPosition.y + Utilities::CHUNK_DEPTH));
	//int topRight = getElevationAtPosition(glm::ivec2(startingPosition.x + Utilities::CHUNK_WIDTH, startingPosition.y + Utilities::CHUNK_DEPTH));
	//for (int z = 0; z < + Utilities::CHUNK_DEPTH; ++z)
	//{
	//	for (int x = 0; x < + Utilities::CHUNK_WIDTH; ++x)
	//	{
	//		int h = bilinearInterpolation(bottomLeft, topLeft, bottomRight, topRight,
	//			0, Utilities::CHUNK_WIDTH,
	//			0, Utilities::CHUNK_DEPTH,
	//			z, x);
	//		 
	//		//m_heightMap[x][z] = h;
	//	}
	//}
}

int Chunk::getElevationAtPosition(const glm::ivec2& position) const
{
	switch (getBiomeType(position.x, position.y))
	{
	case eBiomeType::Plains :
		return getElevationAtPosition(position.x, position.y, PLAINS_LACUNARITY, PLAINS_PERSISTENCE, PLAINS_OCTAVES,
			PLAINS_REDISTRIBUTION, PLAINS_MAX_HEIGHT);
		break;
	case eBiomeType::Desert:
		return getElevationAtPosition(position.x, position.y, DESERT_LACUNARITY, DESERT_PERSISTENCE, DESERT_OCTAVES,
			DESERT_REDISTRIBUTION, DESERT_MAX_HEIGHT);
		break;
	default:
		assert(false);
	}
}

bool Chunk::isPositionOnBiomeTransition(const glm::ivec2& position, int searchRadius) const
{
	eBiomeType currentBiome = getBiomeType(position.x, position.y);
	for (int y = position.y - searchRadius / 2; y <= position.y + searchRadius / 2; y += searchRadius)
	for (int x = position.x - searchRadius / 2; x <= position.x + searchRadius / 2; x += searchRadius)
	{
		if (getBiomeType(x, y) != currentBiome)
		{
			return true;
		}
	}

	return false;
}

int Chunk::addNoiseToPosition(int height)
{
	


	//float elevation = 0.0f;

	//float persistence = terrainPersistence;
	//float lacunarity = terrainLacunarity;
	//for (int i = 0; i < terrainOctaves; ++i)
	//{
	//	elevation += persistence * glm::perlin(glm::vec2(ex * lacunarity, ey * lacunarity));

	//	persistence /= 2.0f;
	//	lacunarity *= 2.0f;
	//}

	//persistence = terrainPersistence;
	//float total = 0.0f;
	//for (int i = 0; i < terrainOctaves; ++i)
	//{
	//	total += persistence;
	//	persistence /= 2.0f;
	//}

	//elevation /= total;

	//elevation = (elevation + 1) / 2;
	//elevation = glm::pow(elevation, terrainRedistribution);
	//elevation = elevation * (float)maxHeight - 1;

	//return elevation;
}

bool Chunk::isPositionInLocalBounds(const glm::ivec3& position) const
{
	return (position.x >= 0 &&
		position.y >= 0 &&
		position.z >= 0 &&
		position.x < Utilities::CHUNK_WIDTH &&
		position.y < Utilities::CHUNK_HEIGHT &&
		position.z < Utilities::CHUNK_DEPTH);
}

bool Chunk::isCubeAtLocalPosition(const glm::ivec3& position, eCubeType cubeType) const
{
	assert(isPositionInLocalBounds(position));
	return m_chunk[converTo1D(position)] == static_cast<char>(cubeType);
}

int Chunk::getElevationAtPosition(int x, int y, float terrainLacunarity, float terrainPersistence,
	float terrainOctaves, float terrainRedistribution, int maxHeight) const
{
	double ex = x / static_cast<float>(Utilities::MAP_SIZE);
	double ey = y / static_cast<float>(Utilities::MAP_SIZE);

	float elevation = 0.0f;

	float persistence = terrainPersistence;
	float lacunarity = terrainLacunarity;
	for (int i = 0; i < terrainOctaves; ++i)
	{
		elevation += persistence * glm::perlin(glm::vec2(ex * lacunarity, ey * lacunarity));

		persistence /= 2.0f;
		lacunarity *= 2.0f;
	}
	
	persistence = terrainPersistence;
	float total = 0.0f;
	for (int i = 0; i < terrainOctaves; ++i)
	{
		total += persistence;
		persistence /= 2.0f;
	}

	elevation /= total;

	elevation = (elevation + 1) / 2;
	elevation = glm::pow(elevation, terrainRedistribution);
	elevation = elevation * (float)maxHeight - 1;

	return elevation;
}

//elevation = (elevation - -1) / (1 - -1) * (Utilities::CHUNK_HEIGHT - 1) + 1;

eBiomeType Chunk::getBiomeType(int x, int z) const
{
	double bx = x / 2000.0f;
	double by = z / 2000.0f;

	float biomeType = 0.0f;
	float moisturePersistence = BIOME_PERSISTENCE;
	float moistureLacunarity = BIOME_LACUNARITY;
	for (int i = 0; i < BIOME_OCTAVES; ++i)
	{
		biomeType += moisturePersistence * glm::perlin(glm::vec2(bx * moistureLacunarity, by * moistureLacunarity));

		moisturePersistence /= 2.0f;
		moistureLacunarity *= 2.0f;
	}

	float persistence = BIOME_PERSISTENCE;
	float total = 0.0f;
	for (int i = 0; i < BIOME_OCTAVES; ++i)
	{
		total += persistence;
		persistence /= 2.0f;
	}

	biomeType /= total;
	biomeType = (biomeType + 1) / 2;

	if (biomeType >= 0.55f)
	{
		return eBiomeType::Plains;
	}
	else 
	{
		return eBiomeType::Desert;
	}
	//else if (biomeType >= 0.20f)
	//{
	//	return eBiomeType::Mountains;
	//}
	//else
	//{
	//	return eBiomeType::Islands;
	//}
}