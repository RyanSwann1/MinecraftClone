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
	m_next(nullptr)
{}

Chunk::Chunk(const glm::ivec3& startingPosition, ChunkManager& chunkManager)
	: m_inUse(false),
	m_startingPosition(startingPosition),
	m_endingPosition(),
	m_chunk(),
	m_next(nullptr)
{
	regen(m_startingPosition, chunkManager);
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

const CubeDetails& Chunk::getCubeDetailsWithoutBoundsCheck(const glm::ivec3& position) const
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
		m_chunk[position.x - m_startingPosition.x][position.y][position.z - m_startingPosition.z].type = static_cast<char>(cubeType);
	}
}

void Chunk::setNext(Chunk* chunk)
{
	m_next = chunk;
}

void Chunk::reuse(const glm::ivec3& startingPosition, ChunkManager& chunkManager)
{
	for (int z = 0; z < Utilities::CHUNK_DEPTH; ++z)
	{
		for (int y = 0; y < Utilities::CHUNK_HEIGHT; ++y)
		{
			for (int x = 0; x < Utilities::CHUNK_WIDTH; ++x)
			{
				m_chunk[x][y][z] = CubeDetails();
			}
		}
	}

	m_inUse = true;
	m_startingPosition = startingPosition;
	regen(m_startingPosition, chunkManager);	
}

void Chunk::release()
{
	m_inUse = false;
	m_startingPosition = glm::ivec3();
	m_endingPosition = glm::ivec3();
}

void Chunk::regen(const glm::ivec3& startingPosition, ChunkManager& chunkManager)
{
	for (int z = startingPosition.z; z < startingPosition.z + Utilities::CHUNK_DEPTH; ++z)
	{
		for (int x = startingPosition.x; x < startingPosition.x + Utilities::CHUNK_WIDTH; ++x)
		{
			double nx = (x) / (Utilities::VISIBILITY_DISTANCE * 1.0f) - 0.5f;
			double ny = (z) / (Utilities::VISIBILITY_DISTANCE * 1.0f) - 0.5f;

			//float elevation = std::abs(glm::perlin(glm::vec2(nx, ny)));
			
			float elevation = std::abs(1 * glm::perlin(glm::vec2(1 * nx, 1 * ny)));
			elevation += std::abs(0.5 * glm::perlin(glm::vec2(nx * 2, ny * 2)));
			elevation += std::abs(0.25 * glm::perlin(glm::vec2(nx * 4, ny * 4)));

			//elevation = glm::pow(elevation, 5.f);
			elevation = (float)Utilities::CHUNK_HEIGHT - 1.0f - (elevation * (float)Utilities::CHUNK_HEIGHT);
			//elevation = glm::pow(elevation, 1.25f);
			elevation = Utilities::clampTo(elevation, 0.0f, (float)Utilities::CHUNK_HEIGHT - 1.0f);

			//glm::perlin(glm::vec2(nx * 1.25f, ny * 1.25f))) * 16;
			eCubeType cubeType;
			if (elevation <= Utilities::STONE_MAX_HEIGHT)
			{
				cubeType = eCubeType::Stone;
			}
			//else if (elevation <= Utilities::WATER_MAX_HEIGHT)
			//{
			//	cubeType = eCubeType::Water;
			//}
			else if (elevation <= Utilities::SAND_MAX_HEIGHT)
			{
				cubeType = eCubeType::Sand;
			}
			else
			{
				cubeType = eCubeType::Grass;
			}

			glm::ivec3 positionOnGrid(x - startingPosition.x, (int)elevation, z - startingPosition.z);
			//std::cout << elevation << "\n";
			m_chunk[positionOnGrid.x][(int)elevation][positionOnGrid.z] = CubeDetails(cubeType);

			for (int y = (int)elevation - 1; y >= 0; --y)
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

				m_chunk[positionOnGrid.x][(int)y][positionOnGrid.z] = CubeDetails(cubeType);
			}
		}
	}

	m_endingPosition = glm::ivec3(startingPosition.x + Utilities::CHUNK_WIDTH, startingPosition.y + Utilities::CHUNK_HEIGHT,
		startingPosition.z + Utilities::CHUNK_DEPTH);

	//Fill with Water
	for (int z = 0; z < Utilities::CHUNK_DEPTH; ++z)
	{
		for (int x = 0; x < Utilities::CHUNK_WIDTH; ++x)
		{
			if (m_chunk[x][Utilities::WATER_MAX_HEIGHT][z].type == static_cast<char>(eCubeType::Invalid))
			{
				m_chunk[x][Utilities::WATER_MAX_HEIGHT][z].type = static_cast<char>(eCubeType::Water);
			}
		}
	}

	//Fill with trees
	for (int z = 0; z < Utilities::CHUNK_DEPTH; ++z)
	{
		for (int x = 0; x < Utilities::CHUNK_WIDTH; ++x)
		{
			for (int y = Utilities::CHUNK_HEIGHT - 1; y >= Utilities::SAND_MAX_HEIGHT; --y)
			{
				if (m_chunk[x][y][z].type == static_cast<char>(eCubeType::Grass))
				{
					if (Utilities::getRandomNumber(0, 1200) >= Utilities::TREE_SPAWN_CHANCE)
					{
						int maxTreeHeight = Utilities::getRandomNumber(Utilities::TREE_MIN_HEIGHT, Utilities::TREE_MAX_HEIGHT);
						int treeHeight = 0;
						for (int i = 1; i <= maxTreeHeight; ++i)
						{
							if (y + i < Utilities::CHUNK_HEIGHT - 1)
							{
								treeHeight = i;
								m_chunk[x][y + i][z].type = static_cast<char>(eCubeType::TreeStump);
							}
							else
							{
								break;
							}
						}

						/* worldPosition.x = m_startingPosition.x + worldPosition.x;
						worldPosition.z = m_startingPosition.z + worldPosition.z;*/
						//Utilities::convertToWorldPosition(worldPosition, m_startingPosition);
						//Utilities::convertToWorldPosition(worldPosition)
						spawnLeaves(glm::ivec3(x, y + treeHeight + 1, z), chunkManager);
					}

					break;
				}
			}
		}
	}
}

void Chunk::spawnLeaves(const glm::ivec3& startingPosition, ChunkManager& chunkManager)
{
	if (isPositionInLocalBounds(startingPosition)) 
	{
		m_chunk[startingPosition.x][startingPosition.y][startingPosition.z].type = static_cast<char>(eCubeType::Leaves);
	}

	for (int x = startingPosition.x - 3; x <= startingPosition.x + 3; ++x)
	{
		glm::ivec3 position(x, startingPosition.y, startingPosition.z);
		if (isPositionInLocalBounds(position))
		{
			if (m_chunk[position.x][position.y][position.z].type == static_cast<char>(eCubeType::Invalid)) 
			{
				m_chunk[position.x][position.y][position.z].type = static_cast<char>(eCubeType::Leaves);
			}
		}
		else
		{
			chunkManager.changeCubeAtPosition(glm::ivec3(m_startingPosition.x + position.x, startingPosition.y, m_startingPosition.z + position.z), 
				eCubeType::Leaves);
		}
	}

	for (int z = startingPosition.z - 3; z <= startingPosition.z + 3; ++z)
	{
		glm::ivec3 position(startingPosition.x, startingPosition.y, z);
		if (isPositionInLocalBounds(position))
		{
			if (m_chunk[position.x][position.y][position.z].type == static_cast<char>(eCubeType::Invalid))
			{
				m_chunk[position.x][position.y][position.z].type = static_cast<char>(eCubeType::Leaves);
			}
		}
		else
		{
			chunkManager.changeCubeAtPosition(glm::ivec3(m_startingPosition.x + position.x, startingPosition.y, m_startingPosition.z + position.z),
				eCubeType::Leaves);
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
