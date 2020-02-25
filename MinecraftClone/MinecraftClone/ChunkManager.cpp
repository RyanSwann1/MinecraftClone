#include "ChunkManager.h"
#include "Utilities.h"
#include "VertexBuffer.h"
#include "Texture.h"
#include "VertexArray.h"
#include "CubeID.h"
#include "Rectangle.h"
#include <iostream>

ChunkManager::ChunkManager()
	: m_chunks(),
	m_chunkMeshRegenerateQueue()
{}

void ChunkManager::generateInitialChunks(glm::ivec3 playerPosition, std::unordered_map<glm::ivec3, VertexArray>& VAOs, const Texture& texture)
{
	for (int z = playerPosition.z - Utilities::VISIBILITY_DISTANCE; z < playerPosition.z + Utilities::VISIBILITY_DISTANCE; z += Utilities::CHUNK_DEPTH)
	{
		for (int x = playerPosition.x - Utilities::VISIBILITY_DISTANCE; x < playerPosition.x + Utilities::VISIBILITY_DISTANCE; x += Utilities::CHUNK_WIDTH)
		{
			glm::ivec3 chunkStartingPosition = Utilities::getClosestChunkStartingPosition(glm::ivec3(x, 0, z));
			if (m_chunks.find(chunkStartingPosition) == m_chunks.cend())
			{
				m_chunks.emplace(std::piecewise_construct,
					std::forward_as_tuple(chunkStartingPosition),
					std::forward_as_tuple(chunkStartingPosition));

				VAOs.emplace(std::piecewise_construct,
					std::forward_as_tuple(chunkStartingPosition),
					std::forward_as_tuple());
			}
		}
	}

	for (const auto& chunk : m_chunks)
	{
		auto VAO = VAOs.find(chunk.second.getStartingPosition());
		assert(VAO != VAOs.cend());
		if (VAO != VAOs.cend())
		{
			generateChunkMesh(VAO->second, texture, chunk.second);
		}
	}
}

void ChunkManager::update(const Rectangle& visibilityRect, std::unordered_map<glm::ivec3, VertexArray>& VAOs, glm::vec3 playerPosition,
	const Texture& texture)
{
	deleteChunks(visibilityRect, VAOs);
	addChunks(visibilityRect, VAOs, playerPosition, texture);
	regenChunks(visibilityRect, VAOs, texture);
}

void ChunkManager::addCubeFace(VertexBuffer& vertexBuffer, const Texture& texture, CubeDetails cubeDetails, eCubeSide cubeSide,
	int& elementArrayBufferIndex, glm::ivec3 cubePosition) const
{
	switch (cubeSide)
	{
	case eCubeSide::Front:
		for (glm::vec3 i : Utilities::CUBE_FACE_FRONT)
		{
			i += cubePosition;
			vertexBuffer.positions.push_back({ i.x, i.y, i.z });
		}

		switch (static_cast<eCubeType>(cubeDetails.type))
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
			i += cubePosition;
			vertexBuffer.positions.push_back({ i.x, i.y, i.z });
		}

		switch (static_cast<eCubeType>(cubeDetails.type))
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
			i += cubePosition;
			vertexBuffer.positions.push_back({ i.x, i.y, i.z });
		}

		switch (static_cast<eCubeType>(cubeDetails.type))
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
			i += cubePosition;
			vertexBuffer.positions.push_back({ i.x, i.y, i.z });
		}

		switch (static_cast<eCubeType>(cubeDetails.type))
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
			i += cubePosition;
			vertexBuffer.positions.push_back({ i.x, i.y, i.z });
		}

		switch (static_cast<eCubeType>(cubeDetails.type))
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
			i += cubePosition;
			vertexBuffer.positions.push_back({ i.x, i.y, i.z });
		}

		switch (static_cast<eCubeType>(cubeDetails.type))
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

bool ChunkManager::isCubeAtPosition(glm::ivec3 position) const
{
	glm::ivec3 closestChunkStartingPosition = Utilities::getClosestChunkStartingPosition(position);
	auto cIter = m_chunks.find(closestChunkStartingPosition);
	if (cIter != m_chunks.cend() && cIter->second.isPositionInBounds(position) && 
		static_cast<eCubeType>(cIter->second.getCubeDetails(position).type) != eCubeType::Invalid)
	{
		return true;
	}

	return false;
}

bool ChunkManager::isChunkAtPosition(glm::ivec3 position) const
{
	glm::ivec3 closestChunkStartingPosition = Utilities::getClosestChunkStartingPosition(position);
	auto cIter = m_chunks.find(closestChunkStartingPosition);
	return cIter != m_chunks.cend();
}

void ChunkManager::generateChunkMesh(VertexArray& vertexArray, const Texture& texture, const Chunk& chunk)
{
	int elementArrayBufferIndex = 0;
	glm::ivec3 chunkStartingPosition = chunk.getStartingPosition();
	bool regenChunk = false;

	for (int z = chunkStartingPosition.z; z < chunkStartingPosition.z + Utilities::CHUNK_DEPTH; ++z)
	{
		for (int y = chunkStartingPosition.y; y < chunkStartingPosition.y + Utilities::CHUNK_HEIGHT; ++y)
		{
			for (int x = chunkStartingPosition.x; x < chunkStartingPosition.x + Utilities::CHUNK_WIDTH; ++x)
			{
				glm::ivec3 position(x, y, z);
				if (static_cast<eCubeType>(chunk.getCubeDetails(position).type) == eCubeType::Invalid)
				{
					continue;
				}

				if (isChunkAtPosition(glm::ivec3(x - 1, y, z)) && !isCubeAtPosition(glm::ivec3(x - 1, y, z)))
				{
					addCubeFace(vertexArray.m_vertexBuffer, texture, chunk.getCubeDetails(position), eCubeSide::Left, elementArrayBufferIndex, position);
				}
				else if (!regenChunk && !isChunkAtPosition(glm::ivec3(x - 1, y, z)))
				{
					regenChunk = true;
				}

				if (isChunkAtPosition(glm::ivec3(x + 1, y, z)) && !isCubeAtPosition(glm::ivec3(x + 1, y, z)))
				{
					addCubeFace(vertexArray.m_vertexBuffer, texture, chunk.getCubeDetails(position), eCubeSide::Right, elementArrayBufferIndex, position);
				}
				else if (!regenChunk && !isChunkAtPosition(glm::ivec3(x + 1, y, z)))
				{
					regenChunk = true;
				}

				if (!isCubeAtPosition(glm::ivec3(x, y + 1, z)))
				{
					addCubeFace(vertexArray.m_vertexBuffer, texture, chunk.getCubeDetails(position), eCubeSide::Top, elementArrayBufferIndex, position);
				}

				if (isChunkAtPosition(glm::ivec3(x, y, z -1)) && !isCubeAtPosition(glm::ivec3(x, y, z - 1)))
				{
					addCubeFace(vertexArray.m_vertexBuffer, texture, chunk.getCubeDetails(position), eCubeSide::Back, elementArrayBufferIndex, position);
				}
				else if (!regenChunk && !isChunkAtPosition(glm::ivec3(x, y, z - 1)))
				{
					regenChunk = true;
				}

				if (isChunkAtPosition(glm::ivec3(x, y, z + 1)) && !isCubeAtPosition(glm::ivec3(x, y, z + 1)))
				{
					addCubeFace(vertexArray.m_vertexBuffer, texture, chunk.getCubeDetails(position), eCubeSide::Front, elementArrayBufferIndex, position);
				}
				else if (!regenChunk && !isChunkAtPosition(glm::ivec3(x, y, z + 1)))
				{
					regenChunk = true;
				}
			}
		}
	}
	
	if (regenChunk)
	{
		m_chunkMeshRegenerateQueue.push_back(chunkStartingPosition);
	}

	vertexArray.init();
	vertexArray.m_vertexBuffer.textCoords.clear();
	vertexArray.m_vertexBuffer.positions.clear();
}

void ChunkManager::deleteChunks(const Rectangle& visibilityRect, std::unordered_map<glm::ivec3, VertexArray>& VAOs)
{
	for (auto chunk = m_chunks.begin(); chunk != m_chunks.end();)
	{
		Rectangle chunkAABB(glm::ivec2(chunk->second.getStartingPosition().x, chunk->second.getStartingPosition().z) +
			glm::ivec2(Utilities::CHUNK_WIDTH / 2.0f, Utilities::CHUNK_DEPTH / 2.0f), 16);
		if (!visibilityRect.contains(chunkAABB))
		{
			glm::ivec3 chunkStartingPosition = chunk->second.getStartingPosition();
			auto VAO = VAOs.find(chunkStartingPosition);
			assert(VAO != VAOs.end());
			if (VAO != VAOs.end())
			{
				VAOs.erase(VAO);
			}

			auto chunkToRegen = std::find_if(m_chunkMeshRegenerateQueue.begin(), m_chunkMeshRegenerateQueue.end(), [chunkStartingPosition](const auto& position)
				{
					return position == chunkStartingPosition;
				});
			if (chunkToRegen != m_chunkMeshRegenerateQueue.end())
			{
				m_chunkMeshRegenerateQueue.erase(chunkToRegen);
			}

			chunk = m_chunks.erase(chunk);
		}
		else
		{
			++chunk;
		}
	}
}

void ChunkManager::addChunks(const Rectangle& visibilityRect, std::unordered_map<glm::ivec3, VertexArray>& VAOs, glm::ivec3 playerPosition, const Texture& texture)
{
	std::queue<const Chunk*> newlyAddedChunks;
	glm::ivec3 closestChunkStartingPosition = Utilities::getClosestChunkStartingPosition(playerPosition);	
	for (int z = closestChunkStartingPosition.z - Utilities::VISIBILITY_DISTANCE; z < closestChunkStartingPosition.z + Utilities::VISIBILITY_DISTANCE; z += Utilities::CHUNK_DEPTH)
	{
		for (int x = closestChunkStartingPosition.x - Utilities::VISIBILITY_DISTANCE; x < closestChunkStartingPosition.x + Utilities::VISIBILITY_DISTANCE; x += Utilities::CHUNK_WIDTH)
		{
			glm::ivec3 position(x, 0, z);
			if (m_chunks.find(position) == m_chunks.cend())
			{
				auto newChunk = m_chunks.emplace(std::piecewise_construct,
					std::forward_as_tuple(position),
					std::forward_as_tuple(position)).first;

				newlyAddedChunks.push(&newChunk->second);
			}
		}
	}

	while (!newlyAddedChunks.empty())
	{
		const Chunk* chunk = newlyAddedChunks.front();
		newlyAddedChunks.pop();

		auto newVAO = VAOs.emplace(std::piecewise_construct,
			std::forward_as_tuple(chunk->getStartingPosition()),
			std::forward_as_tuple()).first;

		generateChunkMesh(newVAO->second, texture, *chunk);
	}
}

void ChunkManager::regenChunks(const Rectangle& visibilityRect, std::unordered_map<glm::ivec3, VertexArray>& VAOs, const Texture& texture)
{
	for (auto chunkStartPosition = m_chunkMeshRegenerateQueue.begin(); chunkStartPosition != m_chunkMeshRegenerateQueue.end();)
	{
		auto chunk = m_chunks.find(*chunkStartPosition);
		if (chunk != m_chunks.cend())
		{
			if (m_chunks.find(glm::ivec3(chunkStartPosition->x - Utilities::CHUNK_WIDTH, chunkStartPosition->y, chunkStartPosition->z)) != m_chunks.cend() &&
				m_chunks.find(glm::ivec3(chunkStartPosition->x + Utilities::CHUNK_WIDTH, chunkStartPosition->y, chunkStartPosition->z)) != m_chunks.cend() &&
				m_chunks.find(glm::ivec3(chunkStartPosition->x, chunkStartPosition->y,  chunkStartPosition->z - Utilities::CHUNK_DEPTH)) != m_chunks.cend() &&
				m_chunks.find(glm::ivec3(chunkStartPosition->x, chunkStartPosition->y, chunkStartPosition->z + Utilities::CHUNK_DEPTH)) != m_chunks.cend())
			{
				auto VAO = VAOs.find(chunk->second.getStartingPosition());
				assert(VAO != VAOs.end());
				if (VAO != VAOs.end())
				{
					VAO->second.reset();
					generateChunkMesh(VAO->second, texture, chunk->second);
				}

				chunkStartPosition = m_chunkMeshRegenerateQueue.erase(chunkStartPosition);
			}
			else
			{
				++chunkStartPosition;
			}

			/*if (m_chunks.find(glm::ivec3( glm::ivec2(chunkStartingPosition->x - Utilities::CHUNK_WIDTH, chunkStartingPosition->y)) != m_chunks.cend() &&
				m_chunks.find(glm::ivec2(chunkStartingPosition->x + Utilities::CHUNK_WIDTH, chunkStartingPosition->y)) != m_chunks.cend() &&
				m_chunks.find(glm::ivec2(chunkStartingPosition->x, chunkStartingPosition->y - Utilities::CHUNK_DEPTH)) != m_chunks.cend() &&
				m_chunks.find(glm::ivec2(chunkStartingPosition->x, chunkStartingPosition->y + Utilities::CHUNK_DEPTH)) != m_chunks.cend())
			{
				glm::vec3 startPosition(chunk->second.getStartingPosition().x, 0, chunk->second.getStartingPosition().z);

				auto VAO = VAOs.find(glm::ivec2(chunk->second.getStartingPosition().x, chunk->second.getStartingPosition().z));
				assert(VAO != VAOs.end());
				if (VAO != VAOs.end())
				{
					VAO->second.reset();
					generateChunkMesh(VAO->second, texture, chunk->second);
				}

				chunkStartingPosition = m_chunkMeshRegenerateQueue.erase(chunkStartingPosition);
			}
			else
			{
				++chunkStartingPosition;
			}*/
		}
		else
		{
			chunkStartPosition = m_chunkMeshRegenerateQueue.erase(chunkStartPosition);
		}
	}
}