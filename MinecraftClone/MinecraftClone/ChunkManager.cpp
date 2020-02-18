#include "ChunkManager.h"
#include "Utilities.h"
#include "VertexBuffer.h"
#include "Texture.h"
#include "VertexArray.h"
#include "CubeID.h"
#include "Rectangle.h"
#include <iostream>

ChunkManager::ChunkManager()
	: m_chunks()
{}

void ChunkManager::removeCubeAtPosition(glm::vec3 cameraPosition, glm::vec3 rayCastPosition)
{
	glm::vec3 vBetween = rayCastPosition - cameraPosition;
	
	for (int i = 20; i > 0; --i)
	{
		glm::vec3 distance = vBetween / static_cast<float>(i);
		glm::ivec3 position = cameraPosition + distance;
		for (Chunk& chunk : m_chunks)
		{
			if (chunk.isPositionInBounds(position) &&
				chunk.getCubeDetailsAtPosition(glm::ivec3(position.x, position.y, position.z)).type != eCubeType::Invalid)
			{
				std::cout << "Camera Position\n";
				std::cout << cameraPosition.x << " " << cameraPosition.y << " " << cameraPosition.z << "\n";

				std::cout << "Racyast Position\n";
				std::cout << position.x << " " << position.y << " " << position.z << "\n";
				chunk.removeCubeAtPosition(position);
				handleAdjacentDestroyedBlock(position, chunk);
				m_chunkMeshRegenerateQueue.push(&chunk);
				return;
			}
		}
	}
}

void ChunkManager::generateInitialChunks(glm::vec3 startingPosition, int chunkCount, std::vector<VertexArray>& VAOs, std::vector<VertexBuffer>& VBOs)
{
	glm::ivec2 position(startingPosition.x / 16, startingPosition.z / 16);
	for (int y = position.y - Utilities::VISIBILITY_DISTANCE; y <= Utilities::VISIBILITY_DISTANCE; y += 16)
	{
		for (int x = position.x - Utilities::VISIBILITY_DISTANCE; x <= Utilities::VISIBILITY_DISTANCE; x += 16)
		{
			m_chunks.emplace_back(glm::ivec3(x, 0, y));
			VAOs.emplace_back();
			VBOs.emplace_back();
		}
	}
}

void ChunkManager::generateChunkMeshes(std::vector<VertexArray>& VAOs, std::vector<VertexBuffer>& VBOs, const Texture& texture) const
{
	int i = 0;
	for (auto& chunk : m_chunks)
	{
		generateChunkMesh(VAOs[i], VBOs[i], texture, chunk);
		++i;
	}
}

void ChunkManager::handleChunkMeshRegenerationQueue(std::vector<VertexArray>& VAOs, std::vector<VertexBuffer>& VBOs, const Texture& texture)
{
	while (!m_chunkMeshRegenerateQueue.empty())
	{
		const Chunk* chunk = m_chunkMeshRegenerateQueue.front();
		m_chunkMeshRegenerateQueue.pop();
		assert(chunk);
		if (!chunk)
		{
			continue;
		}

		glm::vec3 chunkStartingPosition = chunk->getStartingPosition();
		auto VBO = std::find_if(VBOs.begin(), VBOs.end(), [chunkStartingPosition](const auto& vertexBuffer)
			{ return vertexBuffer.m_owningChunkStartingPosition == chunkStartingPosition; });
		assert(VBO != VBOs.end());
		glDeleteBuffers(1, &VBO->positionsID);
		glDeleteBuffers(1, &VBO->textCoordsID);
		glDeleteBuffers(1, &VBO->indiciesID);
		VBO->clear();

		auto VAO = std::find_if(VAOs.begin(), VAOs.end(), [chunkStartingPosition](const auto& vertexArray)
			{ return vertexArray.getOwningChunkStartingPosition() == chunkStartingPosition; });
		assert(VAO != VAOs.end());

		generateChunkMesh(*VAO, *VBO, texture, *chunk);
	}
}

void ChunkManager::update(const Rectangle& visibilityRect, std::vector<VertexArray>& VAOs, std::vector<VertexBuffer>& VBOs, glm::vec3 playerPosition,
	const Texture& texture)
{
	//https://silentmatt.com/rectangle-intersection/
	//Delete out of bounds chunks
	for (auto chunk = m_chunks.begin(); chunk != m_chunks.end();)
	{
		Rectangle chunkAABB(glm::ivec2(chunk->getStartingPosition().x, chunk->getStartingPosition().z) +
			glm::ivec2(Utilities::CHUNK_WIDTH / 2.0f, Utilities::CHUNK_DEPTH / 2.0f), 8);
		if (!visibilityRect.contains(chunkAABB))
		{
			glm::vec3 chunkStartingPosition = chunk->getStartingPosition();
			auto VBO = std::find_if(VBOs.begin(), VBOs.end(), [chunkStartingPosition](const auto& vertexBuffer)
				{ return vertexBuffer.m_owningChunkStartingPosition == chunkStartingPosition; });

			assert(VBO != VBOs.end());
			glDeleteBuffers(1, &VBO->positionsID);
			glDeleteBuffers(1, &VBO->textCoordsID);
			glDeleteBuffers(1, &VBO->indiciesID);
			VBOs.erase(VBO);

			auto VAO = std::find_if(VAOs.begin(), VAOs.end(), [chunkStartingPosition](const auto& vertexArray)
				{ return vertexArray.getOwningChunkStartingPosition() == chunkStartingPosition; });
			assert(VAO != VAOs.end());
			VAOs.erase(VAO);

			std::cout << "Chunk Deleted\n";
			std::cout << chunkStartingPosition.x << " " << chunkStartingPosition.z << "\n";
			chunk = m_chunks.erase(chunk);
		}
		else
		{
			++chunk;
		}
	}

	glm::ivec2 position(playerPosition.x / 16, playerPosition.z / 16);
	for (int y = position.y - Utilities::VISIBILITY_DISTANCE; y <= Utilities::VISIBILITY_DISTANCE; y += 16)
	{
		for (int x = position.x - Utilities::VISIBILITY_DISTANCE; x <= Utilities::VISIBILITY_DISTANCE; x += 16)
		{
			glm::ivec2 chunkSpawnPosition(x, y);
			auto chunk = std::find_if(m_chunks.cbegin(), m_chunks.cend(), [chunkSpawnPosition](const auto& chunk)
			{ 
				return chunkSpawnPosition == glm::ivec2(chunk.getStartingPosition().x, chunk.getStartingPosition().z);
			});

			if (chunk == m_chunks.cend())
			{
				std::cout << "Added\n";
				std::cout << chunkSpawnPosition.x << " " << chunkSpawnPosition.y << "\n";
				m_chunks.emplace_back(glm::ivec3(chunkSpawnPosition.x, 0, chunkSpawnPosition.y));
				VAOs.emplace_back();
				VBOs.emplace_back();

				generateChunkMesh(VAOs.back(), VBOs.back(), texture, m_chunks.back());
			}
		}
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
			texture.getTextCoords(eCubeFaceID::Dirt, vertexBuffer.textCoords);
			break;
		}
		break;
	}

	for (unsigned int i : Utilities::CUBE_FACE_INDICIES)
	{
		vertexBuffer.indicies.push_back(i + elementArrayBufferIndex);
	}

	elementArrayBufferIndex += Utilities::CUBE_FACE_INDICIE_COUNT;
}

bool ChunkManager::isCubeAtPosition(glm::vec3 position) const
{
	for (const Chunk& chunk : m_chunks)
	{
		if (chunk.isPositionInBounds(position) && 
			chunk.getCubeDetails(glm::ivec3(position.x, position.y, position.z)).type != eCubeType::Invalid)
		{
			return true;
		}
	}

	return false;
}

bool ChunkManager::isCubeAtPosition(glm::ivec3 position) const
{
	for (const Chunk& chunk : m_chunks)
	{
		if (chunk.isPositionInBounds(position) &&
			chunk.getCubeDetails(glm::ivec3(position.x, position.y, position.z)).type != eCubeType::Invalid)
		{
			return true;
		}
	}

	return false;
}

void ChunkManager::generateChunkMesh(VertexArray& vertexArray, VertexBuffer& vertexBuffer, const Texture& texture, const Chunk& chunk) const
{
	int elementArrayBufferIndex = 0;

	glm::ivec3 chunkStartingPosition = chunk.getStartingPosition();
	vertexBuffer.m_owningChunkStartingPosition = chunkStartingPosition;
	for (int y = chunkStartingPosition.y; y < chunkStartingPosition.y + 16; ++y)
	{
		for (int x = chunkStartingPosition.x; x < chunkStartingPosition.x + 16; ++x)
		{
			for (int z = chunkStartingPosition.z; z < chunkStartingPosition.z + 16; ++z)
			{
				if (chunk.getCubeDetails(glm::ivec3(x, y, z)).type == eCubeType::Invalid)
				{
					continue;
				}

				if (!isCubeAtPosition(glm::ivec3(x - 1, y, z)))
				{
					addCubeFace(vertexBuffer, texture, chunk.getCubeDetails(glm::vec3(x, y, z)), eCubeSide::Left, elementArrayBufferIndex);
				}
				if (!isCubeAtPosition(glm::ivec3(x + 1, y, z)))
				{
					addCubeFace(vertexBuffer, texture, chunk.getCubeDetails(glm::vec3(x, y, z)), eCubeSide::Right, elementArrayBufferIndex);
				}
				if (!isCubeAtPosition(glm::ivec3(x, y - 1, z)))
				{
					addCubeFace(vertexBuffer, texture, chunk.getCubeDetails(glm::vec3(x, y, z)), eCubeSide::Bottom, elementArrayBufferIndex);
				}
				if (!isCubeAtPosition(glm::ivec3(x, y + 1, z)))
				{
					addCubeFace(vertexBuffer, texture, chunk.getCubeDetails(glm::vec3(x, y, z)), eCubeSide::Top, elementArrayBufferIndex);
				}
				if (!isCubeAtPosition(glm::ivec3(x, y, z - 1)))
				{
					addCubeFace(vertexBuffer, texture, chunk.getCubeDetails(glm::vec3(x, y, z)), eCubeSide::Back, elementArrayBufferIndex);
				}
				if (!isCubeAtPosition(glm::ivec3(x, y, z + 1)))
				{
					addCubeFace(vertexBuffer, texture, chunk.getCubeDetails(glm::vec3(x, y, z)), eCubeSide::Front, elementArrayBufferIndex);
				}
			}
		}
	}

	vertexArray.init(vertexBuffer);
}

void ChunkManager::handleAdjacentDestroyedBlock(glm::ivec3 position, const Chunk& owningChunk)
{
	for (const auto& chunk : m_chunks)
	{
		if (chunk.getStartingPosition() == owningChunk.getStartingPosition())
		{
			continue;
		}
		
		if (chunk.isPositionInBounds(glm::ivec3(position.x - 1, position.y, position.z)) ||
			chunk.isPositionInBounds(glm::ivec3(position.x + 1, position.y, position.z)) ||
			chunk.isPositionInBounds(glm::ivec3(position.x, position.y - 1, position.z)) ||
			chunk.isPositionInBounds(glm::ivec3(position.x, position.y + 1, position.z)) ||
			chunk.isPositionInBounds(glm::ivec3(position.x, position.y, position.z - 1)) ||
			chunk.isPositionInBounds(glm::ivec3(position.x, position.y, position.z + 1)))
		{
			m_chunkMeshRegenerateQueue.push(&chunk);
			break;
		}
	}
}