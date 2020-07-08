#pragma once

#include "glm/glm.hpp"

class Chunk;
struct VertexArray;
struct VertexBuffer;
struct NeighbouringChunks;
enum class eCubeType;
enum class eDestroyCubeIndex;
namespace MeshGenerator
{
	void generateVoxelSelectionMesh(VertexBuffer& mesh, const glm::vec3& position);
	void generateDestroyBlockMesh(VertexBuffer& destroyBlockMesh, eDestroyCubeIndex destroyCubeIndex, const glm::vec3& position);
	void generateChunkMesh(VertexArray& chunkMesh, const Chunk& chunk, const NeighbouringChunks& neighbouringChunks);
	void generatePickUpMesh(VertexBuffer& pickUpMesh, eCubeType cubeType);
}