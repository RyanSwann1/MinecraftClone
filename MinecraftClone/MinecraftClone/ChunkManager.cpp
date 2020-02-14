#include "ChunkManager.h"
#include "Utilities.h"
#include "VertexBuffer.h"
#include "Texture.h"

ChunkManager::ChunkManager()
	: m_chunks()
{}

void ChunkManager::generateChunks(glm::vec3 startingPosition)
{
	for (int x = 0; x < 128; x += 8)
	{
		for (int z = 0; z < 128; z += 8)
		{
			m_chunks.emplace_back(glm::vec3(x, 0, z));
		}
	}
}

void ChunkManager::generateChunkMeshes(VertexBuffer& vertexBuffer, const Texture& texture, int& elementArrayBufferIndex) const
{
	for (const Chunk& chunk : m_chunks)
	{
		glm::vec3 chunkStartingPosition = chunk.getStartingPosition();
		for (int y = chunkStartingPosition.y; y < chunkStartingPosition.y + 8; ++y)
		{
			for (int x = chunkStartingPosition.x; x < chunkStartingPosition.x + 8; ++x)
			{
				for (int z = chunkStartingPosition.z; z < chunkStartingPosition.z + 8; ++z)
				{	
					addCube(vertexBuffer, texture, glm::vec3(x, y, z), elementArrayBufferIndex);
					elementArrayBufferIndex += 24;
				}
			}
		}
	}
}

void ChunkManager::addCube(VertexBuffer& vertexBuffer, const Texture& texture, glm::vec3 startPosition, int& elementArrayBufferIndex) const
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

bool ChunkManager::isBlockAir(glm::vec3 position) const
{
	for (const Chunk& chunk : m_chunks)
	{
		if (chunk.isPositionInChunk(position))
		{
			return true;
		}
	}

	return false;
}