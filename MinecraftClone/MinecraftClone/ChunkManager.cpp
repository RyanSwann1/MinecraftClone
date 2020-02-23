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

void ChunkManager::generateInitialChunks(glm::vec3 playerPosition, int chunkCount, std::vector<VertexArray>& VAOs, std::vector<VertexBuffer>& VBOs)
{
	for (int y = playerPosition.z - Utilities::VISIBILITY_DISTANCE; y < playerPosition.z + Utilities::VISIBILITY_DISTANCE; y += Utilities::CHUNK_DEPTH)
	{
		for (int x = playerPosition.x - Utilities::VISIBILITY_DISTANCE; x < playerPosition.x + Utilities::VISIBILITY_DISTANCE; x += Utilities::CHUNK_WIDTH)
		{
			glm::vec2 chunkStartingPosition = Utilities::getClosestChunkStartingPosition(glm::vec2(x, y));
			if (m_chunks.find(chunkStartingPosition) == m_chunks.cend())
			{
				std::shared_ptr<Chunk> chunk = std::make_shared<Chunk>(glm::ivec3(chunkStartingPosition.x, 0, chunkStartingPosition.y));
				m_chunks[glm::ivec2(chunkStartingPosition.x, chunkStartingPosition.y)] = chunk;
				VAOs.emplace_back();
				VBOs.emplace_back();
			}
		}
	}
}

void ChunkManager::generateChunkMeshes(std::vector<VertexArray>& VAOs, std::vector<VertexBuffer>& VBOs, const Texture& texture)
{
	int i = 0;
	for (auto& chunk : m_chunks)
	{
		generateChunkMesh(VAOs[i], VBOs[i], texture, *chunk.second);
		++i;
	}
}

void ChunkManager::update(const Rectangle& visibilityRect, std::vector<VertexArray>& VAOs, std::vector<VertexBuffer>& VBOs, glm::vec3 playerPosition,
	const Texture& texture)
{
	deleteChunks(visibilityRect, VAOs, VBOs);
	if (!m_recycledChunks.empty())
	{
		addChunks(visibilityRect, VAOs, VBOs, playerPosition, texture);
	}
	regenChunks(visibilityRect, VAOs, VBOs, playerPosition, texture);
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

bool ChunkManager::isCubeAtPosition(glm::vec3 position) const
{
	glm::vec2 closestChunkStartingPosition = Utilities::getClosestChunkStartingPosition(glm::vec2(position.x, position.z));
	auto cIter = m_chunks.find(closestChunkStartingPosition);
	if (cIter != m_chunks.cend() && cIter->second->isPositionInBounds(position) && 
		static_cast<eCubeType>(cIter->second->getCubeDetails(position).type) != eCubeType::Invalid)
	{
		return true;
	}

	return false;
}

bool ChunkManager::isCubeAtPosition(glm::ivec3 position) const
{
	glm::vec2 closestChunkStartingPosition = Utilities::getClosestChunkStartingPosition(glm::vec2(position.x, position.z));
	auto cIter = m_chunks.find(closestChunkStartingPosition);
	if (cIter != m_chunks.cend() && cIter->second->isPositionInBounds(position) && 
		static_cast<eCubeType>(cIter->second->getCubeDetails(position).type) != eCubeType::Invalid)
	{
		return true;
	}

	return false;
}

bool ChunkManager::isChunkAtPosition(glm::ivec2 position) const
{
	glm::vec2 closestChunkStartingPosition = Utilities::getClosestChunkStartingPosition(position);
	auto cIter = m_chunks.find(closestChunkStartingPosition);
	return cIter != m_chunks.cend();
}

void ChunkManager::generateChunkMesh(VertexArray& vertexArray, VertexBuffer& vertexBuffer, const Texture& texture, const Chunk& chunk)
{
	int elementArrayBufferIndex = 0;

	glm::ivec3 chunkStartingPosition = chunk.getStartingPosition();
	vertexBuffer.m_owningChunkStartingPosition = chunkStartingPosition;
	
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

				if (isChunkAtPosition(glm::ivec2(x - 1, z)) && !isCubeAtPosition(glm::ivec3(x - 1, y, z)))
				{
					addCubeFace(vertexBuffer, texture, chunk.getCubeDetails(position), eCubeSide::Left, elementArrayBufferIndex, position);
				}
				else if (!regenChunk && !isChunkAtPosition(glm::ivec2(x - 1, z)))
				{
					regenChunk = true;
				}

				if (isChunkAtPosition(glm::ivec2(x + 1, z)) && !isCubeAtPosition(glm::ivec3(x + 1, y, z)))
				{
					addCubeFace(vertexBuffer, texture, chunk.getCubeDetails(position), eCubeSide::Right, elementArrayBufferIndex, position);
				}
				else if (!regenChunk && !isChunkAtPosition(glm::ivec2(x + 1, z)))
				{
					regenChunk = true;
				}
				//if (!isCubeAtPosition(glm::ivec3(x, y - 1, z)))
				//{
				//	addCubeFace(vertexBuffer, texture, chunk.getCubeDetails(position), eCubeSide::Bottom, elementArrayBufferIndex);
				//}
				if (!isCubeAtPosition(glm::ivec3(x, y + 1, z)))
				{
					addCubeFace(vertexBuffer, texture, chunk.getCubeDetails(position), eCubeSide::Top, elementArrayBufferIndex, position);
				}

				if (isChunkAtPosition(glm::ivec2(x, z - 1)) && !isCubeAtPosition(glm::ivec3(x, y, z - 1)))
				{
					addCubeFace(vertexBuffer, texture, chunk.getCubeDetails(position), eCubeSide::Back, elementArrayBufferIndex, position);
				}
				else if (!regenChunk && !isChunkAtPosition(glm::ivec2(x, z - 1)))
				{
					regenChunk = true;
				}

				if (isChunkAtPosition(glm::ivec2(x, z + 1)) && !isCubeAtPosition(glm::ivec3(x, y, z + 1)))
				{
					addCubeFace(vertexBuffer, texture, chunk.getCubeDetails(position), eCubeSide::Front, elementArrayBufferIndex, position);
				}
				else if (!regenChunk && !isChunkAtPosition(glm::ivec2(x, z + 1)))
				{
					regenChunk = true;
				}
			}
		}
	}


	if (regenChunk)
	{
		m_chunkMeshRegenerateQueue.push_back(glm::ivec2(chunkStartingPosition.x, chunkStartingPosition.z));
	}
	//If rengen
	//Add to queue and then add the direction from player too

	vertexArray.init(vertexBuffer);
}

void ChunkManager::deleteChunks(const Rectangle& visibilityRect, std::vector<VertexArray>& VAOs, std::vector<VertexBuffer>& VBOs)
{
	//Deleting Chunks
	int deletedCount = 0;
	for (auto chunk = m_chunks.begin(); chunk != m_chunks.end();)
	{
		Rectangle chunkAABB(glm::ivec2(chunk->second->getStartingPosition().x, chunk->second->getStartingPosition().z) +
			glm::ivec2(Utilities::CHUNK_WIDTH / 2.0f, Utilities::CHUNK_DEPTH / 2.0f), 16);
		if (!visibilityRect.contains(chunkAABB))
		{
			glm::vec3 chunkStartingPosition = chunk->second->getStartingPosition();
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

			glm::ivec2 startPosition(chunkStartingPosition.x, chunkStartingPosition.z);
			auto chunkToRegen = std::find_if(m_chunkMeshRegenerateQueue.begin(), m_chunkMeshRegenerateQueue.end(), [startPosition](const auto& position)
			{
				return position == startPosition;
			});
			if (chunkToRegen != m_chunkMeshRegenerateQueue.end())
			{
				m_chunkMeshRegenerateQueue.erase(chunkToRegen);
			}

			++deletedCount;
			std::shared_ptr<Chunk> recycledChunk(chunk->second);
			//std::cout << recycledChunk.use_count() << "\n";
			m_recycledChunks.push(recycledChunk);
			std::cout << "Deleted count: " << deletedCount << "\n";

			chunk = m_chunks.erase(chunk);
			//std::cout << "Deleted Count: " << deletedCount << "\n";
		}
		else
		{
			++chunk;
		}
	}
	
}

void ChunkManager::addChunks(const Rectangle& visibilityRect, std::vector<VertexArray>& VAOs, std::vector<VertexBuffer>& VBOs, glm::vec3 playerPosition, const Texture& texture)
{
	//Adding Chunks
	std::cout << "Player Position:\n";
	std::cout << playerPosition.x << " " << playerPosition.z << "\n";
	std::queue<std::shared_ptr<Chunk>> chunks;
	int addedCount = 0;
	for (int y = playerPosition.z - Utilities::VISIBILITY_DISTANCE; y < playerPosition.z + Utilities::VISIBILITY_DISTANCE; y += Utilities::CHUNK_DEPTH)
	{
		for (int x = playerPosition.x - Utilities::VISIBILITY_DISTANCE; x < playerPosition.x + Utilities::VISIBILITY_DISTANCE; x += Utilities::CHUNK_WIDTH)
		{
			glm::vec2 closestChunkStartingPosition = Utilities::getClosestChunkStartingPosition(glm::vec2(x, y));

			if (m_chunks.find(closestChunkStartingPosition) == m_chunks.cend() && !m_recycledChunks.empty())
			{
				std::cout << "Closest Starting Position\n";
				std::cout << closestChunkStartingPosition.x << " " << closestChunkStartingPosition.y << "\n";

				std::shared_ptr<Chunk> recycledChunk = m_recycledChunks.front();
				m_recycledChunks.pop();
				recycledChunk->reset(glm::ivec3(closestChunkStartingPosition.x, 0, closestChunkStartingPosition.y));
				++addedCount;
				//std::cout << "Added Count\n";
				//std::cout << addedCount << "\n";

				chunks.push(recycledChunk);
				m_chunks[glm::ivec2(recycledChunk->getStartingPosition().x, recycledChunk->getStartingPosition().z)] = recycledChunk;
			}
		}
	}

	while (!chunks.empty())
	{
		std::shared_ptr<Chunk> recycledChunk = chunks.front();
		chunks.pop();
		VAOs.emplace_back();
		VBOs.emplace_back();
		generateChunkMesh(VAOs.back(), VBOs.back(), texture, *recycledChunk);
	}
}

void ChunkManager::regenChunks(const Rectangle& visibilityRect, std::vector<VertexArray>& VAOs, std::vector<VertexBuffer>& VBOs, glm::vec3 playerPosition, const Texture& texture)
{
	for (auto chunkStartingPosition = m_chunkMeshRegenerateQueue.begin(); chunkStartingPosition != m_chunkMeshRegenerateQueue.end();)
	{
		if (m_chunks.find(glm::ivec2(chunkStartingPosition->x, chunkStartingPosition->y)) != m_chunks.cend())
		{
			if (m_chunks.find(glm::ivec2(chunkStartingPosition->x - Utilities::CHUNK_WIDTH, chunkStartingPosition->y)) != m_chunks.cend() &&
				m_chunks.find(glm::ivec2(chunkStartingPosition->x + Utilities::CHUNK_WIDTH, chunkStartingPosition->y)) != m_chunks.cend() &&
				m_chunks.find(glm::ivec2(chunkStartingPosition->x, chunkStartingPosition->y - Utilities::CHUNK_DEPTH)) != m_chunks.cend() &&
				m_chunks.find(glm::ivec2(chunkStartingPosition->x, chunkStartingPosition->y + Utilities::CHUNK_DEPTH)) != m_chunks.cend())
			{
				auto chunk = m_chunks.find(glm::ivec2(chunkStartingPosition->x, chunkStartingPosition->y));

				glm::vec3 startPosition(chunk->second->getStartingPosition().x, 0, chunk->second->getStartingPosition().z);

				auto VBO = std::find_if(VBOs.begin(), VBOs.end(), [startPosition](const auto& vertexBuffer)
				{ return vertexBuffer.m_owningChunkStartingPosition == startPosition; });
				assert(VBO != VBOs.end());
				glDeleteBuffers(1, &VBO->positionsID);
				glDeleteBuffers(1, &VBO->textCoordsID);
				glDeleteBuffers(1, &VBO->indiciesID);
				VBOs.erase(VBO);

				auto VAO = std::find_if(VAOs.begin(), VAOs.end(), [startPosition](const auto& vertexArray)
				{ return vertexArray.getOwningChunkStartingPosition() == startPosition; });
				assert(VAO != VAOs.end());
				VAOs.erase(VAO);

				VAOs.emplace_back();
				VBOs.emplace_back();

				generateChunkMesh(VAOs.back(), VBOs.back(), texture, *chunk->second);
				chunkStartingPosition = m_chunkMeshRegenerateQueue.erase(chunkStartingPosition);
			}
			else
			{
				++chunkStartingPosition;
			}
		}
		else
		{
			chunkStartingPosition = m_chunkMeshRegenerateQueue.erase(chunkStartingPosition);
		}
	}
}
