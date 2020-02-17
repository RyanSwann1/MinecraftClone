#include "Chunk.h"
#include "VertexBuffer.h"
#include "Texture.h"
#include "VertexBuffer.h"
#include "Utilities.h"
#include <iostream>

Chunk::Chunk(glm::ivec3 startingPosition)
	: m_chunk(),
	m_startingPosition(startingPosition)
{
	for (int x = 0; x < 16; ++x)
	{
		for (int y = 0; y < 16; ++y)
		{
			for (int z = 0; z < 16; z++)
			{
				eCubeType cubeType;
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

				m_chunk[x][y][z] = CubeDetails(cubeType, 
					glm::ivec3(x + startingPosition.x, y + startingPosition.y, z + startingPosition.z));	
			}
		}
	}

	m_endingPosition = m_chunk[15][15][15].position;
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