#pragma once

class Chunk;
struct VertexArray;
struct NeighbouringChunks;

void generateChunkMesh(VertexArray& vertexArray, const Chunk& chunk, const NeighbouringChunks& neighbouringChunks);