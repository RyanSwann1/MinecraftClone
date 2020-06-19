#include "Item.h"
#include "ChunkMeshGenerator.h"
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
}

PickUp::PickUp(eCubeType cubeType, const glm::ivec3& destroyedBlockPosition)
	: m_cubeType(cubeType),
	m_position({ destroyedBlockPosition.x + 0.35f, destroyedBlockPosition.y + 0.35f, destroyedBlockPosition.z + 0.35f}),
	m_velocity(10.0f, 10.0f, 10.0f),
	m_vertexArray(),
	m_delete(false)
{
	addItemCubeFace(m_vertexArray.m_opaqueVertexBuffer, cubeType, eCubeSide::Left, m_position);
	addItemCubeFace(m_vertexArray.m_opaqueVertexBuffer, cubeType, eCubeSide::Right, m_position);
	addItemCubeFace(m_vertexArray.m_opaqueVertexBuffer, cubeType, eCubeSide::Top, m_position);
	addItemCubeFace(m_vertexArray.m_opaqueVertexBuffer, cubeType, eCubeSide::Bottom, m_position);
	addItemCubeFace(m_vertexArray.m_opaqueVertexBuffer, cubeType, eCubeSide::Front, m_position);
	addItemCubeFace(m_vertexArray.m_opaqueVertexBuffer, cubeType, eCubeSide::Back, m_position);

	m_vertexArray.m_opaqueVertexBuffer.bindToVAO = true;
}

void PickUp::update(const glm::vec3& playerPosition, float deltaTime)
{
	if (glm::distance(m_position, { playerPosition.x, playerPosition.y, playerPosition.z }) <= 3.5f)
	{
		m_vertexArray.m_opaqueVertexBuffer.clear();
		addItemCubeFace(m_vertexArray.m_opaqueVertexBuffer, m_cubeType, eCubeSide::Left, m_position);
		addItemCubeFace(m_vertexArray.m_opaqueVertexBuffer, m_cubeType, eCubeSide::Right, m_position);
		addItemCubeFace(m_vertexArray.m_opaqueVertexBuffer, m_cubeType, eCubeSide::Top, m_position);
		addItemCubeFace(m_vertexArray.m_opaqueVertexBuffer, m_cubeType, eCubeSide::Bottom, m_position);
		addItemCubeFace(m_vertexArray.m_opaqueVertexBuffer, m_cubeType, eCubeSide::Front, m_position);
		addItemCubeFace(m_vertexArray.m_opaqueVertexBuffer, m_cubeType, eCubeSide::Back, m_position);
		m_vertexArray.m_opaqueVertexBuffer.bindToVAO = true;

		m_position += glm::normalize(glm::vec3(glm::vec3(playerPosition.x, playerPosition.y - 1.0f, playerPosition.z) - m_position)) * m_velocity * deltaTime;		
		
		if (glm::distance(m_position, { playerPosition.x, playerPosition.y - 1.0f, playerPosition.z }) <= 0.5f)
		{
			m_delete = true;
		}
	}
}