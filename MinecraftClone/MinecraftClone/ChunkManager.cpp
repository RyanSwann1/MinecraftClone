#include "ChunkManager.h"
#include "Utilities.h"
#include "VertexBuffer.h"
#include "Texture.h"
#include "VertexArray.h"

ChunkManager::ChunkManager()
	: m_chunks()
{}

void ChunkManager::generateChunks(glm::vec3 startingPosition, int chunkCount)
{
	m_chunks.reserve(chunkCount * chunkCount);
	for (int x = 0; x < 16 * chunkCount; x += 16)
	{
		for (int z = 0; z < 16 * chunkCount; z += 16)
		{
			m_chunks.emplace_back(glm::vec3(x + startingPosition.x, startingPosition.y, z + startingPosition.z));
		}
	}
}

void ChunkManager::generateChunkMeshes(std::vector<VertexArray>& VAOs, std::vector<VertexBuffer>& VBOs, const Texture& texture) const
{
	for (int i = 0; i < 6 * 6; ++i)
	{
		int elementArrayBufferIndex = 0;

		glm::vec3 chunkStartingPosition = m_chunks[i].getStartingPosition();
		for (int y = chunkStartingPosition.y; y < chunkStartingPosition.y + 16; ++y)
		{
			for (int x = chunkStartingPosition.x; x < chunkStartingPosition.x + 16; ++x)
			{
				for (int z = chunkStartingPosition.z; z < chunkStartingPosition.z + 16; ++z)
				{
					if (!isCubeAtPosition(glm::vec3(x - 1, y, z)))
					{
						addCubeFace(VBOs[i], texture, glm::vec3(x, y, z), eCubeSide::Left, elementArrayBufferIndex);
					}
					if (!isCubeAtPosition(glm::vec3(x + 1, y, z)))
					{
						addCubeFace(VBOs[i], texture, glm::vec3(x, y, z), eCubeSide::Right, elementArrayBufferIndex);
					}
					if (!isCubeAtPosition(glm::vec3(x, y - 1, z)))
					{
						addCubeFace(VBOs[i], texture, glm::vec3(x, y, z), eCubeSide::Bottom, elementArrayBufferIndex);
					}
					if (!isCubeAtPosition(glm::vec3(x, y + 1, z)))
					{
						addCubeFace(VBOs[i], texture, glm::vec3(x, y, z), eCubeSide::Top, elementArrayBufferIndex);
					}
					if (!isCubeAtPosition(glm::vec3(x, y, z - 1)))
					{
						addCubeFace(VBOs[i], texture, glm::vec3(x, y, z), eCubeSide::Back, elementArrayBufferIndex);
					}
					if (!isCubeAtPosition(glm::vec3(x, y, z + 1)))
					{
						addCubeFace(VBOs[i], texture, glm::vec3(x, y, z), eCubeSide::Front, elementArrayBufferIndex);
					}
				}
			}
		}

		VAOs[i].addVertexBuffer(VBOs[i]);
	}
}

void ChunkManager::addCube(VertexBuffer& vertexBuffer, const Texture& texture, glm::vec3 startPosition, int& elementArrayBufferIndex) const
{
	for (glm::vec3 i : Utilities::CUBE_FACE_FRONT)
	{
		i += startPosition;
		vertexBuffer.positions.push_back(i.x);
		vertexBuffer.positions.push_back(i.y);
		vertexBuffer.positions.push_back(i.z);
	}
	texture.getTextCoords(eTileID::GrassSide, vertexBuffer.textCoords);

	for (glm::vec3 i : Utilities::CUBE_FACE_BACK)
	{
		i += startPosition;
		vertexBuffer.positions.push_back(i.x);
		vertexBuffer.positions.push_back(i.y);
		vertexBuffer.positions.push_back(i.z);
	}

	texture.getTextCoords(eTileID::GrassSide, vertexBuffer.textCoords);

	for (glm::vec3 i : Utilities::CUBE_FACE_LEFT)
	{
		i += startPosition;
		vertexBuffer.positions.push_back(i.x);
		vertexBuffer.positions.push_back(i.y);
		vertexBuffer.positions.push_back(i.z);
	}
	texture.getTextCoords(eTileID::GrassSide, vertexBuffer.textCoords);

	for (glm::vec3 i : Utilities::CUBE_FACE_RIGHT)
	{
		i += startPosition;
		vertexBuffer.positions.push_back(i.x);
		vertexBuffer.positions.push_back(i.y);
		vertexBuffer.positions.push_back(i.z);
	}

	texture.getTextCoords(eTileID::GrassSide, vertexBuffer.textCoords);

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

	elementArrayBufferIndex += 24;
}

void ChunkManager::addCubeFace(VertexBuffer& vertexBuffer, const Texture& texture, glm::vec3 startPosition, eCubeSide cubeSide,
	int& elementArrayBufferIndex) const
{
	switch (cubeSide)
	{
	case eCubeSide::Front:
		for (glm::vec3 i : Utilities::CUBE_FACE_FRONT)
		{
			i += startPosition;
			vertexBuffer.positions.push_back(i.x);
			vertexBuffer.positions.push_back(i.y);
			vertexBuffer.positions.push_back(i.z);
		}

		if (startPosition.y <= Utilities::STONE_MAX_HEIGHT)
		{
			texture.getTextCoords(eTileID::Stone, vertexBuffer.textCoords);
		}
		else if (startPosition.y <= Utilities::DIRT_MAX_HEIGHT)
		{
			texture.getTextCoords(eTileID::Dirt, vertexBuffer.textCoords);
		}
		else
		{
			texture.getTextCoords(eTileID::GrassSide, vertexBuffer.textCoords);
		}
		break;
	case eCubeSide::Back:
		for (glm::vec3 i : Utilities::CUBE_FACE_BACK)
		{
			i += startPosition;
			vertexBuffer.positions.push_back(i.x);
			vertexBuffer.positions.push_back(i.y);
			vertexBuffer.positions.push_back(i.z);
		}
		if (startPosition.y <= Utilities::STONE_MAX_HEIGHT)
		{
			texture.getTextCoords(eTileID::Stone, vertexBuffer.textCoords);
		}
		else if (startPosition.y <= Utilities::DIRT_MAX_HEIGHT)
		{
			texture.getTextCoords(eTileID::Dirt, vertexBuffer.textCoords);
		}
		else
		{
			texture.getTextCoords(eTileID::GrassSide, vertexBuffer.textCoords);
		}
		
		break;
	case eCubeSide::Left:
		for (glm::vec3 i : Utilities::CUBE_FACE_LEFT)
		{
			i += startPosition;
			vertexBuffer.positions.push_back(i.x);
			vertexBuffer.positions.push_back(i.y);
			vertexBuffer.positions.push_back(i.z);
		}
		if (startPosition.y <= Utilities::STONE_MAX_HEIGHT)
		{
			texture.getTextCoords(eTileID::Stone, vertexBuffer.textCoords);
		}
		else if (startPosition.y <= Utilities::DIRT_MAX_HEIGHT)
		{
			texture.getTextCoords(eTileID::Dirt, vertexBuffer.textCoords);
		}
		else
		{
			texture.getTextCoords(eTileID::GrassSide, vertexBuffer.textCoords);
		}
		
		break;
	case eCubeSide::Right:
		for (glm::vec3 i : Utilities::CUBE_FACE_RIGHT)
		{
			i += startPosition;
			vertexBuffer.positions.push_back(i.x);
			vertexBuffer.positions.push_back(i.y);
			vertexBuffer.positions.push_back(i.z);
		}
		if (startPosition.y <= Utilities::STONE_MAX_HEIGHT)
		{
			texture.getTextCoords(eTileID::Stone, vertexBuffer.textCoords);
		}
		else if (startPosition.y <= Utilities::DIRT_MAX_HEIGHT)
		{
			texture.getTextCoords(eTileID::Dirt, vertexBuffer.textCoords);
		}
		else
		{
			texture.getTextCoords(eTileID::GrassSide, vertexBuffer.textCoords);
		}
		
		break;
	case eCubeSide::Top:
		for (glm::vec3 i : Utilities::CUBE_FACE_TOP)
		{
			i += startPosition;
			vertexBuffer.positions.push_back(i.x);
			vertexBuffer.positions.push_back(i.y);
			vertexBuffer.positions.push_back(i.z);
		}
		if (startPosition.y <= Utilities::STONE_MAX_HEIGHT)
		{
			texture.getTextCoords(eTileID::Stone, vertexBuffer.textCoords);
		}
		else if (startPosition.y <= Utilities::DIRT_MAX_HEIGHT)
		{
			texture.getTextCoords(eTileID::Dirt, vertexBuffer.textCoords);
		}
		else
		{
			texture.getTextCoords(eTileID::Grass, vertexBuffer.textCoords);
		}
		
		break;
	case eCubeSide::Bottom:
		for (glm::vec3 i : Utilities::CUBE_FACE_BOTTOM)
		{
			i += startPosition;
			vertexBuffer.positions.push_back(i.x);
			vertexBuffer.positions.push_back(i.y);
			vertexBuffer.positions.push_back(i.z);
		}
		if (startPosition.y <= Utilities::STONE_MAX_HEIGHT)
		{
			texture.getTextCoords(eTileID::Stone, vertexBuffer.textCoords);
		}
		else 
		{
			texture.getTextCoords(eTileID::Dirt, vertexBuffer.textCoords);
		}
		break;
	}

	for (unsigned int i : Utilities::CUBE_FACE_INDICIES)
	{
		vertexBuffer.indicies.push_back(i + elementArrayBufferIndex);
	}

	elementArrayBufferIndex += 4;
}

bool ChunkManager::isCubeAtPosition(glm::vec3 position) const
{
	for (const Chunk& chunk : m_chunks)
	{
		if (chunk.isPositionInBounds(position))
		{
			return true;
		}
	}

	return false;
}

//glm::vec3 startPosition(x, y, z);
//std::array<bool, static_cast<int>(eCubeSide::Total)> facesGenerated = 
//	{ false, false, false, false, false, false };
//left
//if (isBlockAir(glm::vec3(x - 1, y, z)))
//{
//	facesGenerated[static_cast<int>(eCubeSide::Left)] = true;
//	for (glm::vec3 i : Utilities::CUBE_FACE_LEFT)
//	{
//		i += startPosition;
//		vertexBuffer.positions.push_back(i.x);
//		vertexBuffer.positions.push_back(i.y);
//		vertexBuffer.positions.push_back(i.z);
//	}
//				
//	texture.getTextCoords(eTileID::GrassSide, vertexBuffer.textCoords);
//}
//Right
//if (isBlockAir(glm::vec3(x + 1, y, z)))
//{
//	facesGenerated[static_cast<int>(eCubeSide::Right)] = true;
//	for (glm::vec3 i : Utilities::CUBE_FACE_RIGHT)
//	{
//		i += startPosition;
//		vertexBuffer.positions.push_back(i.x);
//		vertexBuffer.positions.push_back(i.y);
//		vertexBuffer.positions.push_back(i.z);
//	}

//	texture.getTextCoords(eTileID::GrassSide, vertexBuffer.textCoords);
//}
//Bottom
//if (isBlockAir(glm::vec3(x, y - 1, z)))
//{
//	facesGenerated[static_cast<int>(eCubeSide::Bottom)] = true;
//	for (glm::vec3 i : Utilities::CUBE_FACE_BOTTOM)
//	{
//		i += startPosition;
//		vertexBuffer.positions.push_back(i.x);
//		vertexBuffer.positions.push_back(i.y);
//		vertexBuffer.positions.push_back(i.z);
//	}

//	texture.getTextCoords(eTileID::Dirt, vertexBuffer.textCoords);
//}
//Top
//if (isBlockAir(glm::vec3(x, y + 1, z)))
//{
//	facesGenerated[static_cast<int>(eCubeSide::Top)] = true;
//	for (glm::vec3 i : Utilities::CUBE_FACE_TOP)
//	{
//		i += startPosition;
//		vertexBuffer.positions.push_back(i.x);
//		vertexBuffer.positions.push_back(i.y);
//		vertexBuffer.positions.push_back(i.z);
//	}

//	texture.getTextCoords(eTileID::Grass, vertexBuffer.textCoords);
//}
//Front
//if (isBlockAir(glm::vec3(x, y, z - 1)))
//{
//	facesGenerated[static_cast<int>(eCubeSide::Back)] = true;
//	for (glm::vec3 i : Utilities::CUBE_FACE_BACK)
//	{
//		i += startPosition;
//		vertexBuffer.positions.push_back(i.x);
//		vertexBuffer.positions.push_back(i.y);
//		vertexBuffer.positions.push_back(i.z);
//	}

//	texture.getTextCoords(eTileID::GrassSide, vertexBuffer.textCoords);
//}
//Back
//if (isBlockAir(glm::vec3(x, y, z + 1)))
//{
//	facesGenerated[static_cast<int>(eCubeSide::Front)] = true;
//	for (glm::vec3 i : Utilities::CUBE_FACE_FRONT)
//	{
//		i += startPosition;
//		vertexBuffer.positions.push_back(i.x);
//		vertexBuffer.positions.push_back(i.y);
//		vertexBuffer.positions.push_back(i.z);
//	}

//	texture.getTextCoords(eTileID::GrassSide, vertexBuffer.textCoords);
//}

//for (int i = 0; i < static_cast<int>(eCubeSide::Total); ++i)
//{
//	if (!facesGenerated[i])
//	{
//		eCubeSide cubeSide = static_cast<eCubeSide>(i);
//		addCubeFace(vertexBuffer, texture, startPosition, cubeSide);
//	}
//}

//for (unsigned int i : Utilities::CUBE_INDICIES)
//{
//	vertexBuffer.indicies.push_back(i + elementArrayBufferIndex);
//}