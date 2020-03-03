#include "Chunk.h"
#include "VertexBuffer.h"
#include "Texture.h"
#include "VertexBuffer.h"
#include "glm/gtc/noise.hpp"
#include <iostream>
#include <algorithm>

Chunk::Chunk()
	: m_inUse(false),
	m_startingPosition(),
	m_endingPosition(),
	m_chunk(),
	m_next(nullptr)
{}

Chunk::Chunk(const glm::ivec3& startingPosition)
	: m_inUse(false),
	m_startingPosition(startingPosition),
	m_endingPosition(),
	m_chunk(),
	m_next(nullptr)
{
	regen(m_startingPosition);
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
				m_chunk[x][y][z] = CubeDetails();
			}
		}
	}

	m_inUse = true;
	m_startingPosition = startingPosition;
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
					int randNumb = Utilities::getRandomNumber(0, 1000);
					if (randNumb >= Utilities::TREE_SPAWN_CHANCE)
					{
						m_chunk[x][y + 1][z].type = static_cast<char>(eCubeType::TreeStump);
					}
					break;
				}
			}
		}
	}
}