#include "Chunk.h"
#include "VertexBuffer.h"
#include "Texture.h"
#include "VertexBuffer.h"
#include "glm/gtc/noise.hpp"
#include "ChunkManager.h"
#include <iostream>
#include <algorithm>

Chunk::Chunk()
	: m_inUse(false),
	m_startingPosition(),
	m_endingPosition(),
	m_chunk(),
	m_next(nullptr),
	m_AABB()
{}

Chunk::Chunk(const glm::ivec3& startingPosition)
	: m_inUse(false),
	m_startingPosition(startingPosition),
	m_endingPosition(),
	m_chunk(),
	m_next(nullptr),
	m_AABB(glm::ivec2(m_startingPosition.x, m_startingPosition.z) +
		glm::ivec2(Utilities::CHUNK_WIDTH / 2.0f, Utilities::CHUNK_DEPTH / 2.0f), 16)
{
	regen(m_startingPosition);
}

Chunk::Chunk(Chunk&& orig) noexcept
	: m_inUse(orig.m_inUse),
	m_startingPosition(orig.m_startingPosition),
	m_endingPosition(orig.m_endingPosition),
	m_chunk(orig.m_chunk),
	m_next(orig.m_next),
	m_AABB(orig.m_AABB)
{
	orig.m_next = nullptr;
}

Chunk& Chunk::operator=(Chunk&& orig) noexcept
{
	m_inUse = orig.m_inUse;
	m_startingPosition = orig.m_startingPosition;
	m_endingPosition = orig.m_endingPosition;
	m_chunk = orig.m_chunk;
	m_next = orig.m_next;
	m_AABB = orig.m_AABB;

	orig.m_next = nullptr;
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

char Chunk::getCubeDetailsWithoutBoundsCheck(const glm::ivec3& position) const
{
	return m_chunk[position.x - m_startingPosition.x]
		[position.y - m_startingPosition.y]
		[position.z - m_startingPosition.z];
}

Chunk* Chunk::getNext()
{
	return m_next;
}

void Chunk::changeCubeAtPosition(const glm::vec3& position, eCubeType cubeType)
{
	assert(isPositionInBounds(position));
	if (isPositionInBounds(position))
	{
		m_chunk[position.x - m_startingPosition.x][position.y][position.z - m_startingPosition.z] = static_cast<char>(cubeType);
	}
}

void Chunk::setNext(Chunk* chunk)
{
	m_next = chunk;
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

void Chunk::regen(const glm::ivec3& startingPosition)
{
	for (int z = startingPosition.z; z < startingPosition.z + Utilities::CHUNK_DEPTH; ++z)
	{
		for (int x = startingPosition.x; x < startingPosition.x + Utilities::CHUNK_WIDTH; ++x)
		{
			double ex = (x) / (Utilities::MAP_SIZE * 1.0f) - 0.5f;
			double ey = (z) / (Utilities::MAP_SIZE * 1.0f) - 0.5f;
			
			float elevation = std::abs(1 * glm::perlin(glm::vec2(5.0f * ex, 5.0f * ey)));
			elevation += std::abs(0.5 * glm::perlin(glm::vec2(ex * 15.0f, ey * 15.0f)));
			elevation += std::abs(0.25 * glm::perlin(glm::vec2(ex * 30.0f, ey * 30.0f)));

			elevation = glm::pow(elevation, 2.5f);
			elevation = elevation * (float)Utilities::CHUNK_HEIGHT;
			elevation = Utilities::clampTo(elevation, 0.0f, (float)Utilities::CHUNK_HEIGHT - 1.0f);

			double mx = (x) / (Utilities::MAP_SIZE * 1.0f) - 0.5f;
			double my = (z) / (Utilities::MAP_SIZE * 1.0f) - 0.5f;
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

					m_chunk[positionOnGrid.x][(int)y][positionOnGrid.z] = static_cast<char>(cubeType);
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

					m_chunk[positionOnGrid.x][(int)y][positionOnGrid.z] = static_cast<char>(cubeType);
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
			if (m_chunk[x][Utilities::WATER_MAX_HEIGHT][z] == static_cast<char>(eCubeType::Invalid))
			{
				m_chunk[x][Utilities::WATER_MAX_HEIGHT][z] = static_cast<char>(eCubeType::Water);
			}
		}
	}
}

void Chunk::spawnTrees()
{
	int currentTreesPlanted = 0;
	int maxAllowedTrees = Utilities::getRandomNumber(0, Utilities::MAX_TREE_PER_CHUNK);
	if (maxAllowedTrees > 0)
	{
		for (int z = Utilities::MAX_LEAVES_DISTANCE + 1; z < Utilities::CHUNK_DEPTH - Utilities::MAX_LEAVES_DISTANCE - 1; ++z)
		{
			for (int x = Utilities::MAX_LEAVES_DISTANCE + 1; x < Utilities::CHUNK_WIDTH - Utilities::MAX_LEAVES_DISTANCE - 1; ++x)
			{
				if (currentTreesPlanted < maxAllowedTrees && Utilities::getRandomNumber(0, 1400) >= Utilities::TREE_SPAWN_CHANCE)
				{
					for (int y = Utilities::CHUNK_HEIGHT - Utilities::TREE_MAX_HEIGHT - Utilities::MAX_LEAVES_DISTANCE; y >= Utilities::SAND_MAX_HEIGHT; --y)
					{
						if (m_chunk[x][y][z]  == static_cast<char>(eCubeType::Grass) &&
							m_chunk[x][y + 1][z] == static_cast<char>(eCubeType::Invalid))
						{
							++currentTreesPlanted;
							int currentTreeHeight = 1;
							int leavesDistanceIndex = 0;
							for (currentTreeHeight; currentTreeHeight <= Utilities::TREE_MAX_HEIGHT; ++currentTreeHeight)
							{
								if (currentTreeHeight >= (Utilities::TREE_MAX_HEIGHT / 2))
								{
									spawnLeaves(glm::ivec3(x, y + currentTreeHeight + 1, z), Utilities::LEAVES_DISTANCES[leavesDistanceIndex]);
									++leavesDistanceIndex;
								}
								if (y + currentTreeHeight < Utilities::CHUNK_HEIGHT - 1)
								{
									m_chunk[x][y + currentTreeHeight][z] = static_cast<char>(eCubeType::TreeStump);
								}
								else
								{
									break;
								}
							}

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
}

void Chunk::spawnCactus()
{
	int totalCactusAdded = 0;
	for (int z = 0; z < Utilities::CHUNK_DEPTH; ++z)
	{
		for (int x = 0; x < Utilities::CHUNK_WIDTH; ++x)
		{
			if (Utilities::getRandomNumber(0, Utilities::CACTUS_SPAWN_CHANCE) >= Utilities::CACTUS_SPAWN_CHANCE &&
				totalCactusAdded < Utilities::MAX_CACTUS_PER_CHUNK)
			{
				for (int y = Utilities::CHUNK_HEIGHT - Utilities::CACTUS_MAX_HEIGHT - 1; y >= Utilities::WATER_MAX_HEIGHT; --y)
				{
					if (m_chunk[x][y][z] == static_cast<char>(eCubeType::Sand) && 
						m_chunk[x][y + 1][z] == static_cast<char>(eCubeType::Invalid))
					{
						++totalCactusAdded;
						int cactusMaxHeight = Utilities::getRandomNumber(Utilities::CACTUS_MIN_HEIGHT, Utilities::CACTUS_MAX_HEIGHT);
						for (int i = 1; i <= cactusMaxHeight; ++i)
						{
							m_chunk[x][y + i][z] = static_cast<char>(eCubeType::Cactus);
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

void Chunk::spawnLeaves(const glm::ivec3& startingPosition, int distance)
{
	for (int z = startingPosition.z - distance; z <= startingPosition.z + distance; ++z)
	{
		for (int x = startingPosition.x - distance; x <= startingPosition.x + distance; ++x)
		{
			glm::ivec3 position(x, startingPosition.y, z);

			if (position != startingPosition &&
				isPositionInLocalBounds(position) && 
				m_chunk[position.x][position.y][position.z] == static_cast<char>(eCubeType::Invalid))
			{
				m_chunk[position.x][position.y][position.z] = static_cast<char>(eCubeType::Leaves);
			}
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
