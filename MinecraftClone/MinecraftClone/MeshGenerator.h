#pragma once

#include "glm/glm.hpp"
#include <vector>
#include <array>

class Chunk;
struct VertexArray;
struct VertexBuffer;
struct NeighbouringChunks;
enum class eCubeSide;
enum class eCubeType;

namespace MeshGenerator
{
	constexpr int CUBE_FACE_INDICIE_COUNT = 4;

	constexpr std::array<unsigned int, 6> CUBE_FACE_INDICIES =
	{
		0, 1, 2,
		2, 3, 0
	};

	constexpr float DEFAULT_LIGHTING_INTENSITY = 1.0f;
	constexpr float TOP_LIGHTING_INTENSITY = 1.0f;
	constexpr float FRONT_FACE_LIGHTING_INTENSITY = 0.8f;
	constexpr float BACK_FACE_LIGHTING_INTENSITY = 0.8f;
	constexpr float LEFT_FACE_LIGHTING_INTENSITY = 0.75f;
	constexpr float RIGHT_FACE_LIGHTING_INTENSITY = 0.75f;
	constexpr float SHADOW_INTENSITY = 0.4f;
	constexpr float BOTTOM_FACE_LIGHTING_INTENSITY = 0.4f;

	void generateChunkMesh(VertexArray& chunkMesh, const Chunk& chunk, const NeighbouringChunks& neighbouringChunks);
	void generatePickUpMesh(VertexBuffer& pickUpMesh, eCubeType cubeType, const glm::vec3& position);
}