#pragma once

class Chunk;
struct VertexArray;
struct NeighbouringChunks;

void generateChunkMesh(VertexArray& chunkMesh, const Chunk& chunk, const NeighbouringChunks& neighbouringChunks);