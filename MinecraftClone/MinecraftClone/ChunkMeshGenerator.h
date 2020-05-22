#pragma once

struct ChunkMeshToGenerate;
struct NeighbouringChunks;

void generateChunkMesh(ChunkMeshToGenerate& chunkMeshToGenerate, const NeighbouringChunks& neighbouringChunks);