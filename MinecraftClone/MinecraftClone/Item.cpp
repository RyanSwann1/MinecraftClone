#include "Item.h"
#include "ChunkMeshGenerator.h"
#include "ChunkManager.h"
#include <iostream>

namespace
{
	constexpr std::array<glm::vec3, 4> PICKUP_CUBE_FACE_FRONT = { glm::vec3(0, 0, 0.25f), glm::vec3(0.25f, 0, 0.25f), glm::vec3(0.25f, 0.25f, 0.25f), glm::vec3(0, 0.25f, 0.25f) };
	constexpr std::array<glm::vec3, 4> PICKUP_CUBE_FACE_BACK = { glm::vec3(0.25f, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0.25f, 0), glm::vec3(0.25f, 0.25f, 0) };

	constexpr std::array<glm::vec3, 4> PICKUP_CUBE_FACE_LEFT = { glm::vec3(0, 0, 0), glm::vec3(0, 0, 0.25f), glm::vec3(0, 0.25f, 0.25f), glm::vec3(0, 0.25f, 0) };
	constexpr std::array<glm::vec3, 4> PICKUP_CUBE_FACE_RIGHT = { glm::vec3(0.25f, 0, 0.25f), glm::vec3(0.25f, 0, 0), glm::vec3(0.25f, 0.25f, 0), glm::vec3(0.25f, 0.25f, 0.25f) };

	constexpr std::array<glm::vec3, 4> PICKUP_CUBE_FACE_TOP = { glm::vec3(0, 0.25f, 0.25f), glm::vec3(0.25f, 0.25f, 0.25f), glm::vec3(0.25f, 0.25f, 0), glm::vec3(0, 0.25f, 0) };
	constexpr std::array<glm::vec3, 4> PICKUP_CUBE_FACE_BOTTOM = { glm::vec3(0, 0, 0), glm::vec3(0.25f, 0, 0), glm::vec3(0.25f, 0, 0.25f), glm::vec3(0, 0, 0.25f) };

	constexpr std::array<glm::vec3, 4> PICKUP_FIRST_DIAGONAL_FACE = { glm::vec3(0, 0, 0), glm::vec3(0.25f, 0, 0.25f), glm::vec3(0.25f, 0.25f, 0.25f), glm::vec3(0, 0.25f, 0) };
	constexpr std::array<glm::vec3, 4> PICKUP_SECOND_DIAGONAL_FACE = { glm::vec3(0, 0, 0.25f), glm::vec3(0.25f, 0, 0), glm::vec3(0.25f, 0.25f, 0), glm::vec3(0, 0.25f, 0.25f) };

	void addItemCubeFace(VertexBuffer& vertexBuffer, eCubeType cubeType, eCubeSide cubeSide, const glm::vec3& cubePosition)
	{
		glm::vec3 position = cubePosition;
		switch (cubeSide)
		{
		case eCubeSide::Front:
			for (const glm::vec3& i : PICKUP_CUBE_FACE_FRONT)
			{
				position += i;
				vertexBuffer.positions.emplace_back(position.x, position.y, position.z);
				position = cubePosition;

				vertexBuffer.lightIntensityVertices.push_back(MeshGenerator::FRONT_FACE_LIGHTING_INTENSITY);
			}

			getTextCoords(vertexBuffer.textCoords, cubeSide, cubeType);

			break;
		case eCubeSide::Back:
			for (const glm::vec3& i : PICKUP_CUBE_FACE_BACK)
			{
				position += i;
				vertexBuffer.positions.emplace_back(position.x, position.y, position.z);
				position = cubePosition;

				vertexBuffer.lightIntensityVertices.push_back(MeshGenerator::BACK_FACE_LIGHTING_INTENSITY);
			}

			getTextCoords(vertexBuffer.textCoords, cubeSide, cubeType);

			break;
		case eCubeSide::Left:
			for (const glm::vec3& i : PICKUP_CUBE_FACE_LEFT)
			{
				position += i;
				vertexBuffer.positions.emplace_back(position.x, position.y, position.z);
				position = cubePosition;

				vertexBuffer.lightIntensityVertices.push_back(MeshGenerator::LEFT_FACE_LIGHTING_INTENSITY);
			}
			getTextCoords(vertexBuffer.textCoords, cubeSide, cubeType);
			break;
		case eCubeSide::Right:
			for (const glm::vec3& i : PICKUP_CUBE_FACE_RIGHT)
			{
				position += i;
				vertexBuffer.positions.emplace_back(position.x, position.y, position.z);
				position = cubePosition;

				vertexBuffer.lightIntensityVertices.push_back(MeshGenerator::RIGHT_FACE_LIGHTING_INTENSITY);
			}

			getTextCoords(vertexBuffer.textCoords, cubeSide, cubeType);

			break;
		case eCubeSide::Top:
			for (const glm::vec3& i : PICKUP_CUBE_FACE_TOP)
			{
				position += i;
				vertexBuffer.positions.emplace_back(position.x, position.y, position.z);
				position = cubePosition;

				vertexBuffer.lightIntensityVertices.push_back(MeshGenerator::TOP_LIGHTING_INTENSITY);
			}
			getTextCoords(vertexBuffer.textCoords, cubeSide, cubeType);

			break;
		case eCubeSide::Bottom:
			for (const glm::vec3& i : PICKUP_CUBE_FACE_BOTTOM)
			{
				position += i;
				vertexBuffer.positions.emplace_back(position.x, position.y, position.z);
				position = cubePosition;

				vertexBuffer.lightIntensityVertices.push_back(MeshGenerator::BOTTOM_FACE_LIGHTING_INTENSITY);
			}

			getTextCoords(vertexBuffer.textCoords, cubeSide, cubeType);

			break;
		}

		for (unsigned int i : MeshGenerator::CUBE_FACE_INDICIES)
		{
			vertexBuffer.indicies.emplace_back(i + vertexBuffer.elementBufferIndex);
		}

		vertexBuffer.elementBufferIndex += MeshGenerator::CUBE_FACE_INDICIE_COUNT;
	}

	constexpr float DRAG_AMOUNT = 0.5f;
}

PickUp::PickUp(eCubeType cubeType, const glm::ivec3& destroyedBlockPosition)
	: m_AABB({ destroyedBlockPosition.x + 0.5f, destroyedBlockPosition.z + 0.5f }, 0.5f),
	m_cubeType(cubeType),
	m_position({ destroyedBlockPosition.x + 0.35f, destroyedBlockPosition.y + 0.35f, destroyedBlockPosition.z + 0.35f}),
	m_velocity(),
	m_movementSpeed(5.0f),
	m_vertexArray(),
	m_delete(false)
{
	glm::vec3 n = glm::normalize(glm::vec3(Globals::getRandomNumber(0, 360), 90, Globals::getRandomNumber(0, 360)));
	m_velocity.x += n.x * 5.0f;
	m_velocity.y += n.y * 2.5f;
	m_velocity.z += n.z * 5.0f;

	addItemCubeFace(m_vertexArray.m_opaqueVertexBuffer, cubeType, eCubeSide::Left, m_position);
	addItemCubeFace(m_vertexArray.m_opaqueVertexBuffer, cubeType, eCubeSide::Right, m_position);
	addItemCubeFace(m_vertexArray.m_opaqueVertexBuffer, cubeType, eCubeSide::Top, m_position);
	addItemCubeFace(m_vertexArray.m_opaqueVertexBuffer, cubeType, eCubeSide::Bottom, m_position);
	addItemCubeFace(m_vertexArray.m_opaqueVertexBuffer, cubeType, eCubeSide::Front, m_position);
	addItemCubeFace(m_vertexArray.m_opaqueVertexBuffer, cubeType, eCubeSide::Back, m_position);

	m_vertexArray.m_opaqueVertexBuffer.bindToVAO = true;
}

void PickUp::update(const glm::vec3& playerPosition, float deltaTime, const ChunkManager& chunkManager)
{
	eCubeType cubeType;
	if (chunkManager.isCubeAtPosition({ std::floor(m_position.x),
	std::floor(m_position.y),
	std::floor(m_position.z) }, cubeType) &&
		!Globals::NON_COLLIDABLE_CUBE_TYPES.isMatch(cubeType))
	{
		m_velocity.y = 0;
		m_position.y += std::abs(m_position.y - 0.35f - (std::floor(m_position.y - 0.35f) + 1));
		m_position.y = std::floor(m_position.y);
	}
	else
	{
		m_velocity.y -= 0.1f;
	}
	
	if (false)
	{
		//Apply sin wave thing - make bobble up & down
	}

	if (glm::distance(m_position, { playerPosition.x, playerPosition.y, playerPosition.z }) <= 2.5f)
	{
		m_velocity += glm::normalize(glm::vec3(glm::vec3(playerPosition.x, playerPosition.y - 1.0f, playerPosition.z) - m_position)) * 5.0f;
	}

	m_position += m_velocity * deltaTime;

	m_velocity.x *= 0.8f;
	m_velocity.z *= 0.8f;

	m_vertexArray.m_opaqueVertexBuffer.clear();
	addItemCubeFace(m_vertexArray.m_opaqueVertexBuffer, m_cubeType, eCubeSide::Left, m_position);
	addItemCubeFace(m_vertexArray.m_opaqueVertexBuffer, m_cubeType, eCubeSide::Right, m_position);
	addItemCubeFace(m_vertexArray.m_opaqueVertexBuffer, m_cubeType, eCubeSide::Top, m_position);
	addItemCubeFace(m_vertexArray.m_opaqueVertexBuffer, m_cubeType, eCubeSide::Bottom, m_position);
	addItemCubeFace(m_vertexArray.m_opaqueVertexBuffer, m_cubeType, eCubeSide::Front, m_position);
	addItemCubeFace(m_vertexArray.m_opaqueVertexBuffer, m_cubeType, eCubeSide::Back, m_position);
	m_vertexArray.m_opaqueVertexBuffer.bindToVAO = true;

	if (glm::distance(m_position, { playerPosition.x, playerPosition.y - 1.0f, playerPosition.z }) <= 0.5f)
	{
		m_delete = true;
	}
}