#include "Chunk.h"
#include "VertexBuffer.h"
#include "Texture.h"
#include "VertexBuffer.h"
#include "Utilities.h"

Chunk::Chunk(glm::vec2 startingPosition, int elementArrayBufferIndex, VertexBuffer& vertexBuffer)
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

void Chunk::addCube(VertexBuffer& vertexBuffer, const Texture& texture, glm::vec3 startPosition, int elementArrayBufferIndex)
{
	for (glm::vec3 i : Utilities::CUBE_FRONT_FACE)
	{
		i += startPosition;
		vertexBuffer.positions.push_back(i.x);
		vertexBuffer.positions.push_back(i.y);
		vertexBuffer.positions.push_back(i.z);
	}
	texture.getTextCoords(eTileID::DirtSide, vertexBuffer.textCoords);

	for (glm::vec3 i : Utilities::CUBE_FACE_FRONT)
	{
		i += startPosition;
		vertexBuffer.positions.push_back(i.x);
		vertexBuffer.positions.push_back(i.y);
		vertexBuffer.positions.push_back(i.z);
	}

	texture.getTextCoords(eTileID::DirtSide, vertexBuffer.textCoords);

	for (glm::vec3 i : Utilities::CUBE_FACE_LEFT)
	{
		i += startPosition;
		vertexBuffer.positions.push_back(i.x);
		vertexBuffer.positions.push_back(i.y);
		vertexBuffer.positions.push_back(i.z);
	}
	texture.getTextCoords(eTileID::DirtSide, vertexBuffer.textCoords);

	for (glm::vec3 i : Utilities::CUBE_FACE_RIGHT)
	{
		i += startPosition;
		vertexBuffer.positions.push_back(i.x);
		vertexBuffer.positions.push_back(i.y);
		vertexBuffer.positions.push_back(i.z);
	}

	texture.getTextCoords(eTileID::DirtSide, vertexBuffer.textCoords);

	for (glm::vec3 i : Utilities::CUBE_FACE_TOP)
	{
		i += startPosition;
		vertexBuffer.positions.push_back(i.x);
		vertexBuffer.positions.push_back(i.y);
		vertexBuffer.positions.push_back(i.z);
	}

	texture.getTextCoords(eTileID::Grass, vertexBuffer.textCoords);

	for (glm::vec3 i : Utilities::CUBE_FACE_BOTTOM)
	{
		i += startPosition;
		vertexBuffer.positions.push_back(i.x);
		vertexBuffer.positions.push_back(i.y);
		vertexBuffer.positions.push_back(i.z);
	}

	texture.getTextCoords(eTileID::Dirt, vertexBuffer.textCoords);

	for (unsigned int i : Utilities::CUBE_INDICIES)
	{
		vertexBuffer.indicies.push_back(i + elementArrayBufferIndex);
	}
}
