#include "Chunk.h"
#include "VertexBuffer.h"
#include "Texture.h"
#include "VertexBuffer.h"

#include "glm/gtc/noise.hpp"
#include <iostream>
#include <algorithm>

Chunk::Chunk(glm::ivec3 startingPosition)
	: m_chunk(),
	m_startingPosition(startingPosition)
{
	for (int z = startingPosition.z; z < startingPosition.z + Utilities::CHUNK_DEPTH; ++z)
	{
		for (int x = startingPosition.x; x < startingPosition.x + Utilities::CHUNK_WIDTH; ++x)
		{
			double nx = (x) / (Utilities::VISIBILITY_DISTANCE * 2.0f) - 0.5f;
			double ny = (z) / (Utilities::VISIBILITY_DISTANCE * 2.0f) - 0.5f;

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
			else if (elevation <= Utilities::DIRT_MAX_HEIGHT)
			{
				cubeType = eCubeType::Dirt;
			}
			else
			{
				cubeType = eCubeType::Grass;
			}

			glm::ivec3 positionOnGrid(x - startingPosition.x, (int)elevation, z - startingPosition.z);
			//std::cout << elevation << "\n";
			m_chunk[positionOnGrid.x][(int)elevation][positionOnGrid.z] = CubeDetails(cubeType, 
				glm::ivec3(x, elevation + startingPosition.y, z));	

			for (int y = (int)elevation - 1; y >= 0; --y)
			{ 
				if (y <= Utilities::STONE_MAX_HEIGHT)
				{
					cubeType = eCubeType::Stone;
				}
				else if (y <= Utilities::DIRT_MAX_HEIGHT)
				{
					cubeType = eCubeType::Dirt;
				}
				else
				{
					cubeType = eCubeType::Grass;
				}

				m_chunk[positionOnGrid.x][(int)y][positionOnGrid.z] = CubeDetails(cubeType,
					glm::ivec3(x, y + startingPosition.y, z));
			}
		}
	}

	m_endingPosition = glm::ivec3(startingPosition.x + Utilities::CHUNK_WIDTH - 1, startingPosition.y + Utilities::CHUNK_HEIGHT - 1, 
		startingPosition.z + Utilities::CHUNK_DEPTH - 1);

	//for (int x = 0; x < 16; ++x)
	//{
	//	for (int y = 15; y >= 0; --y)
	//	{
	//		if()
	//		for (int z = 0; z < 16; ++z)
	//		{
	//			
	//		}
	//	}
	//}



	//for (int x = 0;x < 16; ++x)
	//{
	//	for (int y = 0; y < 16; ++y)
	//	{
	//		for (int z = 0; z < 16; ++z)
	//		{
	//			eCubeType cubeType;
	//			if (y <= Utilities::STONE_MAX_HEIGHT)
	//			{
	//				cubeType = eCubeType::Stone;
	//			}
	//			else if (y <= Utilities::DIRT_MAX_HEIGHT)
	//			{
	//				cubeType = eCubeType::Dirt;
	//			}
	//			else
	//			{
	//				cubeType = eCubeType::Grass;
	//			}

	//			m_chunk[x][y][z] = CubeDetails(cubeType, 
	//				glm::ivec3(x + startingPosition.x, y + startingPosition.y, z + startingPosition.z));	
	//		}
	//	}
	//}

	//m_endingPosition = m_chunk[15][15][15].position;



	//for (int x = 0; x < 16; ++x)
	//{
	//	for (int y = 0; y < 16; ++y)
	//	{
	//		for (int z = 0; z < 16; ++z)
	//		{
	//			eCubeType cubeType;
	//			if (y <= Utilities::STONE_MAX_HEIGHT)
	//			{
	//				cubeType = eCubeType::Stone;
	//			}
	//			else if (y <= Utilities::DIRT_MAX_HEIGHT)
	//			{
	//				cubeType = eCubeType::Dirt;
	//			}
	//			else
	//			{
	//				cubeType = eCubeType::Grass;
	//			}

	//			m_chunk[x][y][z] = CubeDetails(cubeType, 
	//				glm::ivec3(x + startingPosition.x, y + startingPosition.y, z + startingPosition.z));	
	//		}
	//	}
	//}

	//m_endingPosition = m_chunk[15][15][15].position;
}

bool Chunk::isPositionInBounds(glm::ivec2 position) const
{
	glm::ivec2 startingPositionOnGrid(m_startingPosition.x, m_startingPosition.z);
	glm::ivec2 endingPositionOnGrid(m_endingPosition.x, m_endingPosition.z);

	return (position.x >= startingPositionOnGrid.x &&
		position.y >= startingPositionOnGrid.y &&
		position.x <= endingPositionOnGrid.x &&
		position.y <= endingPositionOnGrid.y);
}

bool Chunk::isPositionInBounds(glm::vec3 position) const
{
	return (position.x >= m_startingPosition.x &&
		position.y >= m_startingPosition.y &&
		position.z >= m_startingPosition.z &&
		position.x <= m_endingPosition.x + 1 &&
		position.y <= m_endingPosition.y + 1 &&
		position.z <= m_endingPosition.z + 1);
}

bool Chunk::isPositionInBounds(glm::ivec3 position) const
{
	return (position.x >= m_startingPosition.x &&
		position.y >= m_startingPosition.y &&
		position.z >= m_startingPosition.z &&
		position.x <= m_endingPosition.x &&
		position.y <= m_endingPosition.y &&
		position.z <= m_endingPosition.z);
}

glm::ivec3 Chunk::getEndingPosition() const
{
	return m_endingPosition;
}

glm::ivec3 Chunk::getStartingPosition() const
{
	return m_startingPosition;
}

CubeDetails Chunk::getCubeDetails(glm::ivec3 position) const
{
	assert(isPositionInBounds(position));
	glm::ivec3 positionOnGrid = position - glm::ivec3(m_startingPosition.x, m_startingPosition.y, m_startingPosition.z);
	return m_chunk[positionOnGrid.x][positionOnGrid.y][positionOnGrid.z];
}

CubeDetails Chunk::getCubeDetailsAtPosition(glm::ivec3 position) const
{
	CubeDetails cubeDetails;
	if (position.x >= m_startingPosition.x &&
		position.y >= m_startingPosition.y &&
		position.z >= m_startingPosition.z &&
		position.x <= m_endingPosition.x + 1 &&
		position.y <= m_endingPosition.y + 1 &&
		position.z <= m_endingPosition.z + 1)
	{
		glm::ivec3 positionOnGrid = position - glm::ivec3(m_startingPosition.x, m_startingPosition.y, m_startingPosition.z);
		cubeDetails = m_chunk[positionOnGrid.x][positionOnGrid.y][positionOnGrid.z];
	}

	return cubeDetails;
}

void Chunk::removeCubeAtPosition(glm::ivec3 position)
{
	glm::ivec3 positionOnGrid = position - glm::ivec3(m_startingPosition.x, m_startingPosition.y, m_startingPosition.z);
	m_chunk[positionOnGrid.x][positionOnGrid.y][positionOnGrid.z].type = eCubeType::Invalid;
}