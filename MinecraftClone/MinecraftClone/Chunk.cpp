#include "Chunk.h"
#include "VertexBuffer.h"
#include "Texture.h"
#include "VertexBuffer.h"
#include "Utilities.h"
#include <iostream>

Chunk::Chunk(glm::vec3 startingPosition)
	: m_chunk(),
	m_startingPosition(startingPosition)
{
	for (int x = 0; x < 16; ++x)
	{
		for (int y = 0; y < 16; ++y)
		{
			for (int z = 0; z < 16; z++)
			{
				m_chunk[x][y][z] = glm::vec3(x + startingPosition.x, y + startingPosition.y, z + startingPosition.z);	
			}
		}
	}

	m_endingPosition = m_chunk[15][15][15];
}

bool Chunk::isPositionInBounds(glm::vec3 position) const
{
	return (position.x >= m_startingPosition.x &&
		position.y >= m_startingPosition.y &&
		position.z >= m_startingPosition.z &&
		position.x <= m_endingPosition.x &&
		position.y <= m_endingPosition.y &&
		position.z <= m_endingPosition.z);
}

glm::vec3 Chunk::getStartingPosition() const
{
	return m_startingPosition;
}

bool Chunk::isPositionInChunk(glm::vec3 position) const
{
	for (int x = 0; x < 16; ++x)
	{
		for (int y = 0; y < 16; ++y)
		{
			for (int z = 0; z < 16; ++z)
			{
				if (m_chunk[x][y][z] == position)
				{
					return true;
				}
			}
		}
	}

	return false;
}

const std::array<std::array<std::array<glm::vec3, 16>, 16>, 16> & Chunk::getChunk() const
{
	return m_chunk;
}