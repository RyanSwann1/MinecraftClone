#include "Chunk.h"

Chunk::Chunk(glm::vec2 startingPosition)
	: m_chunk()
{
	for (int x = 0; x < 16; ++x)
	{
		for (int y = 0; y < 16; ++y)
		{
			for (int z = 0; z < 16; z++)
			{
				m_chunk[x][y][z] = glm::vec3(x + startingPosition.x, y, z + startingPosition.y);
			}
		}
	}
}

const std::array<std::array<std::array<glm::vec3, 16>, 16>, 16> & Chunk::getChunk() const
{
	return m_chunk;
}
