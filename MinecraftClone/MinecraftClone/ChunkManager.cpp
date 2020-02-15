#include "ChunkManager.h"
#include "Utilities.h"
#include "VertexBuffer.h"
#include "Texture.h"
#include "VertexArray.h"
#include "CubeID.h"
#include <iostream>

ChunkManager::ChunkManager()
	: m_chunks()
{}

void ChunkManager::removeCubeAtPosition(glm::vec3 position)
{
	glm::ivec3 positionOnGrid((position.x / 1), (position.y / 1), (position.z / 1));
	std::cout << "Position On Grid\n";
	std::cout << positionOnGrid.x << " " << positionOnGrid.y << " " << positionOnGrid.z << "\n";

	for (Chunk& chunk : m_chunks)
	{
		if (chunk.isPositionInBounds(positionOnGrid))
		{
			chunk.removeCubeAtPosition(position);
			break;
		}
	}
}

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
		generateChunkMesh(VAOs[i], VBOs[i], texture, m_chunks[i]);
	}
}

void ChunkManager::handleQueue(std::vector<VertexArray>& VAOs, std::vector<VertexBuffer>& VBOs, const Texture& texture)
{
	while (m_chunkQueue.empty())
	{
		Chunk* chunk = m_chunkQueue.front();
		m_chunkQueue.pop();
		if (!chunk)
		{
			continue;
		}

		glm::vec3 chunkStartingPosition = chunk->getStartingPosition();
		auto VBO = std::find_if(VBOs.begin(), VBOs.end(), [chunkStartingPosition](const auto& vertexBuffer)
			{ return vertexBuffer.m_owningChunkStartingPosition == chunkStartingPosition; });
		assert(VBO != VBOs.end());
		VBO->clear();

		auto VAO = std::find_if(VAOs.begin(), VAOs.end(), [chunkStartingPosition](const auto& vertexArray)
			{ return vertexArray.getOwningChunkStartingPosition() == chunkStartingPosition; });
		assert(VAO != VAOs.end());

		generateChunkMesh(*VAO, *VBO, texture, *chunk);
	}
}

void ChunkManager::addCubeFace(VertexBuffer& vertexBuffer, const Texture& texture, CubeDetails cubeDetails, eCubeSide cubeSide,
	int& elementArrayBufferIndex) const
{
	switch (cubeSide)
	{
	case eCubeSide::Front:
		for (glm::vec3 i : Utilities::CUBE_FACE_FRONT)
		{
			i += cubeDetails.position;
			vertexBuffer.positions.push_back(i.x);
			vertexBuffer.positions.push_back(i.y);
			vertexBuffer.positions.push_back(i.z);
		}

		switch (cubeDetails.type)
		{
		case eCubeType::Stone :
			texture.getTextCoords(eCubeFaceID::Stone, vertexBuffer.textCoords);
			break;
		case eCubeType::Dirt :
			texture.getTextCoords(eCubeFaceID::Dirt, vertexBuffer.textCoords);
			break;
		case eCubeType::Grass :
			texture.getTextCoords(eCubeFaceID::GrassSide, vertexBuffer.textCoords);
			break;
		}
		break;
	case eCubeSide::Back:
		for (glm::vec3 i : Utilities::CUBE_FACE_BACK)
		{
			i += cubeDetails.position;
			vertexBuffer.positions.push_back(i.x);
			vertexBuffer.positions.push_back(i.y);
			vertexBuffer.positions.push_back(i.z);
		}

		switch (cubeDetails.type)
		{
		case eCubeType::Stone:
			texture.getTextCoords(eCubeFaceID::Stone, vertexBuffer.textCoords);
			break;
		case eCubeType::Dirt:
			texture.getTextCoords(eCubeFaceID::Dirt, vertexBuffer.textCoords);
			break;
		case eCubeType::Grass:
			texture.getTextCoords(eCubeFaceID::GrassSide, vertexBuffer.textCoords);
			break;
		}
		break;
	case eCubeSide::Left:
		for (glm::vec3 i : Utilities::CUBE_FACE_LEFT)
		{
			i += cubeDetails.position;
			vertexBuffer.positions.push_back(i.x);
			vertexBuffer.positions.push_back(i.y);
			vertexBuffer.positions.push_back(i.z);
		}

		switch (cubeDetails.type)
		{
		case eCubeType::Stone:
			texture.getTextCoords(eCubeFaceID::Stone, vertexBuffer.textCoords);
			break;
		case eCubeType::Dirt:
			texture.getTextCoords(eCubeFaceID::Dirt, vertexBuffer.textCoords);
			break;
		case eCubeType::Grass:
			texture.getTextCoords(eCubeFaceID::GrassSide, vertexBuffer.textCoords);
			break;
		}
		break;
	case eCubeSide::Right:
		for (glm::vec3 i : Utilities::CUBE_FACE_RIGHT)
		{
			i += cubeDetails.position;
			vertexBuffer.positions.push_back(i.x);
			vertexBuffer.positions.push_back(i.y);
			vertexBuffer.positions.push_back(i.z);
		}

		switch (cubeDetails.type)
		{
		case eCubeType::Stone:
			texture.getTextCoords(eCubeFaceID::Stone, vertexBuffer.textCoords);
			break;
		case eCubeType::Dirt:
			texture.getTextCoords(eCubeFaceID::Dirt, vertexBuffer.textCoords);
			break;
		case eCubeType::Grass:
			texture.getTextCoords(eCubeFaceID::GrassSide, vertexBuffer.textCoords);
			break;
		}
		break;
	case eCubeSide::Top:
		for (glm::vec3 i : Utilities::CUBE_FACE_TOP)
		{
			i += cubeDetails.position;
			vertexBuffer.positions.push_back(i.x);
			vertexBuffer.positions.push_back(i.y);
			vertexBuffer.positions.push_back(i.z);
		}

		switch (cubeDetails.type)
		{
		case eCubeType::Stone:
			texture.getTextCoords(eCubeFaceID::Stone, vertexBuffer.textCoords);
			break;
		case eCubeType::Dirt:
			texture.getTextCoords(eCubeFaceID::Dirt, vertexBuffer.textCoords);
			break;
		case eCubeType::Grass:
			texture.getTextCoords(eCubeFaceID::Grass, vertexBuffer.textCoords);
			break;
		}
		break;
	case eCubeSide::Bottom:
		for (glm::vec3 i : Utilities::CUBE_FACE_BOTTOM)
		{
			i += cubeDetails.position;
			vertexBuffer.positions.push_back(i.x);
			vertexBuffer.positions.push_back(i.y);
			vertexBuffer.positions.push_back(i.z);
		}

		switch (cubeDetails.type)
		{
		case eCubeType::Stone:
			texture.getTextCoords(eCubeFaceID::Stone, vertexBuffer.textCoords);
			break;
		case eCubeType::Dirt:
			texture.getTextCoords(eCubeFaceID::Dirt, vertexBuffer.textCoords);
			break;
		case eCubeType::Grass:
			texture.getTextCoords(eCubeFaceID::Grass, vertexBuffer.textCoords);
			break;
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

void ChunkManager::removeCubeFromChunk(glm::vec3 position)
{
	for (const Chunk& chunk : m_chunks)
	{

	}
}

void ChunkManager::generateChunkMesh(VertexArray& vertexArray, VertexBuffer& vertexBuffer, const Texture& texture, const Chunk& chunk) const
{
	int elementArrayBufferIndex = 0;

	glm::vec3 chunkStartingPosition = chunk.getStartingPosition();
	vertexBuffer.m_owningChunkStartingPosition = chunkStartingPosition;
	for (int y = chunkStartingPosition.y; y < chunkStartingPosition.y + 16; ++y)
	{
		for (int x = chunkStartingPosition.x; x < chunkStartingPosition.x + 16; ++x)
		{
			for (int z = chunkStartingPosition.z; z < chunkStartingPosition.z + 16; ++z)
			{
				if (!isCubeAtPosition(glm::vec3(x - 1, y, z)))
				{
					addCubeFace(vertexBuffer, texture, chunk.getCubeDetails(glm::vec3(x, y, z)), eCubeSide::Left, elementArrayBufferIndex);
				}
				if (!isCubeAtPosition(glm::vec3(x + 1, y, z)))
				{
					addCubeFace(vertexBuffer, texture, chunk.getCubeDetails(glm::vec3(x, y, z)), eCubeSide::Right, elementArrayBufferIndex);
				}
				if (!isCubeAtPosition(glm::vec3(x, y - 1, z)))
				{
					addCubeFace(vertexBuffer, texture, chunk.getCubeDetails(glm::vec3(x, y, z)), eCubeSide::Bottom, elementArrayBufferIndex);
				}
				if (!isCubeAtPosition(glm::vec3(x, y + 1, z)))
				{
					addCubeFace(vertexBuffer, texture, chunk.getCubeDetails(glm::vec3(x, y, z)), eCubeSide::Top, elementArrayBufferIndex);
				}
				if (!isCubeAtPosition(glm::vec3(x, y, z - 1)))
				{
					addCubeFace(vertexBuffer, texture, chunk.getCubeDetails(glm::vec3(x, y, z)), eCubeSide::Back, elementArrayBufferIndex);
				}
				if (!isCubeAtPosition(glm::vec3(x, y, z + 1)))
				{
					addCubeFace(vertexBuffer, texture, chunk.getCubeDetails(glm::vec3(x, y, z)), eCubeSide::Front, elementArrayBufferIndex);
				}
			}
		}
	}

	vertexArray.init(vertexBuffer);
}