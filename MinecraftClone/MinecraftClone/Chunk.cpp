#include "Chunk.h"
#include "VertexBuffer.h"
#include "Texture.h"
#include "VertexBuffer.h"
#include "glm/gtc/noise.hpp"
#include "ChunkManager.h"
#include <iostream>
#include <algorithm>
#include <limits>

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
	m_endingPosition(),
	m_chunk(),
	m_AABB(glm::ivec2(m_startingPosition.x, m_startingPosition.z) +
		glm::ivec2(Utilities::CHUNK_WIDTH / 2.0f, Utilities::CHUNK_DEPTH / 2.0f), 16)
{
	regen(m_startingPosition);
}

Chunk::Chunk(Chunk&& orig) noexcept
	: m_inUse(orig.m_inUse),
	m_startingPosition(orig.m_startingPosition),
	m_endingPosition(orig.m_endingPosition),
	m_chunk(std::move(orig.m_chunk)),
	m_AABB(orig.m_AABB)
{}

Chunk& Chunk::operator=(Chunk&& orig) noexcept
{
	m_inUse = orig.m_inUse;
	m_startingPosition = orig.m_startingPosition;
	m_endingPosition = orig.m_endingPosition;
	m_chunk = std::move(orig.m_chunk);
	m_AABB = orig.m_AABB;


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
	return m_chunk[position.x - m_startingPosition.x]
		[position.y - m_startingPosition.y]
		[position.z - m_startingPosition.z];
}

void Chunk::changeCubeAtLocalPosition(const glm::ivec3& position, eCubeType cubeType)
{
	assert(isPositionInLocalBounds(position));
	m_chunk[position.x][position.y][position.z] = static_cast<char>(cubeType);
}

void Chunk::reuse(const glm::ivec3& startingPosition)
{
	for (int z = 0; z < Utilities::CHUNK_DEPTH; ++z)
	{
		for (int y = 0; y < Utilities::CHUNK_HEIGHT; ++y)
		{
			for (int x = 0; x < Utilities::CHUNK_WIDTH; ++x)
			{
				m_chunk[x][y][z] = char();
			}
		}
	}

	m_inUse = true;
	m_startingPosition = startingPosition;
	m_AABB.reset(glm::ivec2(m_startingPosition.x, m_startingPosition.z) +
		glm::ivec2(Utilities::CHUNK_WIDTH / 2.0f, Utilities::CHUNK_DEPTH / 2.0f), 16);

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

void Chunk::regen(const glm::ivec3& startingPosition)
{
	for (int z = startingPosition.z; z < startingPosition.z + Utilities::CHUNK_DEPTH; ++z)
	{
		for (int x = startingPosition.x; x < startingPosition.x + Utilities::CHUNK_WIDTH; ++x)
		{
			double ex = x / static_cast<float>(Utilities::MAP_SIZE);
			double ey = z / static_cast<float>(Utilities::MAP_SIZE);
			
			int minElevationValue = 0;
			int maxElevationValue = FLT_MAX;
			float elevation = 0.0f;

			float persistence = Utilities::PERSISTENCE;
			float lacunarity = Utilities::LACUNARITY;
			for (int i = 0; i < Utilities::OCTAVES; ++i)
			{
				elevation += persistence * glm::perlin(glm::vec2(ex * lacunarity, ey * lacunarity));
				
				persistence /= 2.0f;
				lacunarity * 2.0f;
			}

			//elevation = std::abs(elevation);
			
			persistence = Utilities::PERSISTENCE;
			float total = 0.0f;
			for (int i = 0; i < Utilities::OCTAVES; ++i)
			{
				total += persistence;
				persistence = persistence /= 2.0f;
			}

			elevation /= total;

			//elevation = glm::pow(elevation, 2.5f);
			elevation = elevation * (float)Utilities::CHUNK_HEIGHT - 1;
			elevation = Utilities::clampTo(elevation, 0.0f, (float)Utilities::CHUNK_HEIGHT - 1.0f);

			double mx = (x) / (Utilities::MAP_SIZE * 2.0f) - 0.5f;
			double my = (z) / (Utilities::MAP_SIZE * 2.0f) - 0.5f;
			float moisture = std::abs(1 * glm::perlin(glm::vec2(15.0f * mx, 15.0f * my)));

			eCubeType cubeType;
			glm::ivec3 positionOnGrid(x - startingPosition.x, (int)elevation, z - startingPosition.z);

			//Desert Biome
			if (moisture >= 0.5f)
			{
				for (int y = (int)elevation; y >= 0; --y)
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
				for (int y = (int)elevation; y >= 0; --y)
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

	m_endingPosition = glm::ivec3(startingPosition.x + Utilities::CHUNK_WIDTH, startingPosition.y + Utilities::CHUNK_HEIGHT,
		startingPosition.z + Utilities::CHUNK_DEPTH);
	
	spawnWater();
	spawnTrees();
	spawnCactus();
}

void Chunk::spawnWater()
{
	for (int z = 0; z < Utilities::CHUNK_DEPTH; ++z)
	{
		for (int x = 0; x < Utilities::CHUNK_WIDTH; ++x)
		{
			glm::ivec3 position(x, Utilities::WATER_MAX_HEIGHT, z);
			if (getCubeAtLocalPosition(position) == static_cast<char>(eCubeType::Invalid))
			{
				changeCubeAtLocalPosition(position, eCubeType::Water);
			}
		}
	}
}

void Chunk::spawnTrees()
{
	int currentTreesPlanted = 0;
	int maxAllowedTrees = Utilities::getRandomNumber(0, Utilities::MAX_TREE_PER_CHUNK);
	if (maxAllowedTrees == 0)
	{
		return;
	}
	for (int z = Utilities::MAX_LEAVES_DISTANCE; z < Utilities::CHUNK_DEPTH - Utilities::MAX_LEAVES_DISTANCE; ++z)
	{
		for (int x = Utilities::MAX_LEAVES_DISTANCE; x < Utilities::CHUNK_WIDTH - Utilities::MAX_LEAVES_DISTANCE; ++x)
		{
			if (currentTreesPlanted < maxAllowedTrees && Utilities::getRandomNumber(0, 1400) >= Utilities::TREE_SPAWN_CHANCE)
			{
				for (int y = Utilities::CHUNK_HEIGHT - Utilities::TREE_HEIGHT - Utilities::MAX_LEAVES_DISTANCE; y >= Utilities::SAND_MAX_HEIGHT; --y)
				{
					glm::ivec3 position(x, y, z);
					if (getCubeAtLocalPosition(position) == static_cast<char>(eCubeType::Grass) &&
						getCubeAtLocalPosition({ x, y + 1, z }) == static_cast<char>(eCubeType::Invalid))
					{
						++currentTreesPlanted;
						spawnLeaves(position);
						spawnTreeStump(position);

						break;
					}
				}
			}
			//Planted all available Trees
			else if (currentTreesPlanted >= maxAllowedTrees)
			{
				return;
			}
		}
	}
}

void Chunk::spawnCactus()
{
	int totalCactusAdded = 0;
	for (int z = 0; z < Utilities::CHUNK_DEPTH; ++z)
	{
		for (int x = 0; x < Utilities::CHUNK_WIDTH; ++x)
		{
			if (totalCactusAdded < Utilities::MAX_CACTUS_PER_CHUNK &&
				Utilities::getRandomNumber(0, Utilities::CACTUS_SPAWN_CHANCE) >= Utilities::CACTUS_SPAWN_CHANCE)
			{
				for (int y = Utilities::CHUNK_HEIGHT - Utilities::CACTUS_MAX_HEIGHT - 1; y >= Utilities::WATER_MAX_HEIGHT; --y)
				{
					if (getCubeAtLocalPosition({ x, y, z }) == static_cast<char>(eCubeType::Sand) &&
						getCubeAtLocalPosition({ x, y + 1, z }) == static_cast<char>(eCubeType::Invalid))
					{
						++totalCactusAdded;

						int cactusMaxHeight = Utilities::getRandomNumber(Utilities::CACTUS_MIN_HEIGHT, Utilities::CACTUS_MAX_HEIGHT);
						for (int i = 1; i <= cactusMaxHeight; ++i)
						{
							changeCubeAtLocalPosition({ x, y + i, z }, eCubeType::Cactus);
						}

						break;
					}
				}
			}
			//Total Cactuses spawned
			else if (totalCactusAdded >= Utilities::MAX_CACTUS_PER_CHUNK)
			{
				return;
			}
		}
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
				if (getCubeAtLocalPosition(position) == static_cast<char>(eCubeType::Invalid))
				{
					changeCubeAtLocalPosition(position, eCubeType::Leaves);
				}
			}
		}
	}
}

void Chunk::spawnTreeStump(const glm::ivec3& startingPosition)
{
	for (int y = startingPosition.y; y <= startingPosition.y + Utilities::TREE_HEIGHT; ++y)
	{
		changeCubeAtLocalPosition({ startingPosition.x, y, startingPosition.z }, eCubeType::TreeStump);
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

char Chunk::getCubeAtLocalPosition(const glm::ivec3 position) const
{
	assert(isPositionInLocalBounds(position));
	return m_chunk[position.x][position.y][position.z];
}
