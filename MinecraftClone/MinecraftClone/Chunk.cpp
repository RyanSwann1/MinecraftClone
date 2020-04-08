#include "Chunk.h"
#include "VertexBuffer.h"
#include "VertexBuffer.h"
#include "glm/gtc/noise.hpp"
#include <iostream>
#include <algorithm>
#include <limits>

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

Chunk::Chunk()
	: m_inUse(false),
	m_startingPosition(),
	m_endingPosition(),
	m_chunk(),
	m_AABB()
{}

Chunk::Chunk(const glm::ivec3& startingPosition)
	: m_inUse(false),
	m_startingPosition(startingPosition),
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
	: m_inUse(orig.m_inUse),
	m_startingPosition(orig.m_startingPosition),
	m_endingPosition(orig.m_endingPosition),
	m_chunk(std::move(orig.m_chunk)),
	m_AABB(orig.m_AABB)
{
	orig.m_inUse = false;
}

Chunk& Chunk::operator=(Chunk&& orig) noexcept
{
	m_inUse = orig.m_inUse;
	m_startingPosition = orig.m_startingPosition;
	m_endingPosition = orig.m_endingPosition;
	m_chunk = std::move(orig.m_chunk);
	m_AABB = orig.m_AABB;

	orig.m_inUse = false;

	return *this;
}

const Rectangle& Chunk::getAABB() const
{
	return m_AABB;
}

bool Chunk::isInUse() const
{
	return m_inUse;
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
	return m_chunk[Utilities::converTo1D(positionOnGrid)];
}

void Chunk::changeCubeAtLocalPosition(const glm::ivec3& position, eCubeType cubeType)
{
	assert(isPositionInLocalBounds(position));
	m_chunk[Utilities::converTo1D(position)] = static_cast<char>(cubeType);
}

void Chunk::reuse(const glm::ivec3& startingPosition)
{	
	memset(&m_chunk, char(), m_chunk.size());

	m_inUse = true;
	m_startingPosition = startingPosition;
	m_endingPosition = glm::ivec3(startingPosition.x + Utilities::CHUNK_WIDTH, startingPosition.y + Utilities::CHUNK_HEIGHT,
		startingPosition.z + Utilities::CHUNK_DEPTH);
	m_AABB.reset(glm::ivec2(m_startingPosition.x, m_startingPosition.z) +
		glm::ivec2(Utilities::CHUNK_WIDTH / 2, Utilities::CHUNK_DEPTH / 2), 16);

	regen(m_startingPosition);	
}

void Chunk::release()
{
	m_inUse = false;
	m_startingPosition = glm::ivec3();
	m_endingPosition = glm::ivec3();
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
	for (int z = startingPosition.z; z < startingPosition.z + Utilities::CHUNK_DEPTH; ++z)
	{
		for (int x = startingPosition.x; x < startingPosition.x + Utilities::CHUNK_WIDTH; ++x)
		{
			int elevation = getElevationValue(x, z);
			float moisture = getMoistureValue(x, z);
			eCubeType cubeType;
			glm::ivec3 positionOnGrid(x - startingPosition.x, elevation, z - startingPosition.z);

			//Desert Biome
			if (moisture >= 0.5f)
			{
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
			//Plains Biome
			else
			{
				for (int y = elevation; y >= 0; --y)
				{
					if (y <= Utilities::STONE_MAX_HEIGHT)
					{
						cubeType = eCubeType::Stone;
					}
					else if (y <= Utilities::SAND_MAX_HEIGHT)
					{
						cubeType = eCubeType::Sand;
					}
					else
					{
						cubeType = eCubeType::Grass;
					}

					changeCubeAtLocalPosition({ positionOnGrid.x, y, positionOnGrid.z }, cubeType);
				}
			}
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
	int attemptsCount = 0;
	int spawnCount = 0;
	while (attemptsCount < Utilities::MAX_TREE_SPAWN_ATTEMPTS && spawnCount < Utilities::MAX_TREE_PER_CHUNK)
	{
		glm::ivec3 spawnPosition;
		spawnPosition.x = Utilities::getRandomNumber(Utilities::MAX_LEAVES_DISTANCE, Utilities::CHUNK_WIDTH - Utilities::MAX_LEAVES_DISTANCE - 1);
		spawnPosition.z = Utilities::getRandomNumber(Utilities::MAX_LEAVES_DISTANCE, Utilities::CHUNK_DEPTH - Utilities::MAX_LEAVES_DISTANCE - 1);

		//Find Spawn Location
		for (int y = Utilities::CHUNK_HEIGHT - Utilities::TREE_HEIGHT - Utilities::MAX_LEAVES_DISTANCE; y >= Utilities::SAND_MAX_HEIGHT; --y)
		{
			spawnPosition.y = y;
			if (isCubeAtLocalPosition(spawnPosition, eCubeType::Grass) &&
				isCubeAtLocalPosition({ spawnPosition.x, spawnPosition.y + 1, spawnPosition.z }, eCubeType::Invalid))
			{
				spawnLeaves(spawnPosition);
				spawnTreeStump(spawnPosition);
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
		spawnPosition.x = Utilities::getRandomNumber(0, Utilities::CHUNK_WIDTH - 1);
		spawnPosition.z = Utilities::getRandomNumber(0, Utilities::CHUNK_DEPTH - 1);

		//Find Spawn Location
		for (int y = Utilities::CHUNK_HEIGHT - 1; y >= 0; --y)
		{
			spawnPosition.y = y;
			if (isCubeAtLocalPosition(spawnPosition, eCubeType::Sand) &&
				isCubeAtLocalPosition({ spawnPosition.x, y + 1, spawnPosition.z }, eCubeType::Invalid))
			{
				//Spawn Cactus
				int cactusHeight = Utilities::getRandomNumber(Utilities::CACTUS_MIN_HEIGHT, Utilities::CACTUS_MAX_HEIGHT);
				for (int i = 1; i <= cactusHeight; ++i)
				{
					changeCubeAtLocalPosition({ spawnPosition.x, spawnPosition.y + i, spawnPosition.z }, eCubeType::Cactus);
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
		spawnPosition.x = Utilities::getRandomNumber(0, Utilities::CHUNK_WIDTH - 1);
		spawnPosition.z = Utilities::getRandomNumber(0, Utilities::CHUNK_DEPTH - 1);

		for (int y = Utilities::CHUNK_HEIGHT - 1; y >= 0; --y)
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

void Chunk::spawnLeaves(const glm::ivec3& startingPosition)
{
	int y = startingPosition.y + Utilities::TREE_HEIGHT / 2; //Starting Height
	for (int distance : Utilities::LEAVES_DISTANCES)
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

void Chunk::spawnTreeStump(const glm::ivec3& startingPosition)
{
	for (int y = startingPosition.y; y <= startingPosition.y + Utilities::TREE_HEIGHT / 2; ++y)
	{
		if (y == startingPosition.y + Utilities::TREE_HEIGHT / 2)
		{
			changeCubeAtLocalPosition({ startingPosition.x, y, startingPosition.z }, eCubeType::LogTop);
		}
		else
		{
			changeCubeAtLocalPosition({ startingPosition.x, y, startingPosition.z }, eCubeType::Log);
		}
	}
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
	return m_chunk[Utilities::converTo1D(position)] == static_cast<char>(cubeType);
}

int Chunk::getElevationValue(int x, int z) const
{
	double ex = x / static_cast<float>(Utilities::MAP_SIZE);
	double ey = z / static_cast<float>(Utilities::MAP_SIZE);

	float elevation = 0.0f;

	float persistence = Utilities::TERRAIN_PERSISTENCE;
	float lacunarity = Utilities::TERRAIN_LACUNARITY;
	for (int i = 0; i < Utilities::TERRAIN_OCTAVES; ++i)
	{
		elevation += persistence * glm::perlin(glm::vec2(ex * lacunarity, ey * lacunarity));

		persistence /= 2.0f;
		lacunarity *= 2.0f;
	}

	persistence = Utilities::TERRAIN_PERSISTENCE;
	float total = 0.0f;
	for (int i = 0; i < Utilities::TERRAIN_OCTAVES; ++i)
	{
		total += persistence;
		persistence /= 2.0f;
	}

	if (elevation < 0)
	{
		elevation = 0.0f;
	}
	elevation = glm::pow(elevation, 1.25f);
	elevation /= total;
	elevation = elevation * (float)Utilities::CHUNK_HEIGHT - 1;
	elevation = Utilities::clampTo(elevation, 0.0f, (float)Utilities::CHUNK_HEIGHT - 1.0f);

	return elevation;
}

float Chunk::getMoistureValue(int x, int z) const
{
	double mx = x / 1000.0f * 1.0f;
	double my = z / 1000.0f * 1.0f;

	float moisture = 0.0f;
	float moisturePersistence = Utilities::MOISTURE_PERSISTENCE;
	float moistureLacunarity = Utilities::MOISTURE_LACUNARITY;
	for (int i = 0; i < Utilities::MOISTURE_OCTAVES; ++i)
	{
		moisture += moisturePersistence * glm::perlin(glm::vec2(mx * moistureLacunarity, my * moistureLacunarity));

		moisturePersistence /= 2.0f;
		moistureLacunarity *= 2.0f;
	}
	if (moisture < 0)
	{
		moisture = 0.0f;
	}

	return moisture;
}
