#pragma once

class Chunk;
struct VertexArray;
struct VertexBuffer;
struct NeighbouringChunks;
enum class eCubeType;
namespace MeshGenerator
{
	void generateChunkMesh(VertexArray& chunkMesh, const Chunk& chunk, const NeighbouringChunks& neighbouringChunks);
	void generatePickUpMesh(VertexBuffer& pickUpMesh, eCubeType cubeType);
}