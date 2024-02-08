#ifndef TERRAIN_H
#define TERRAIN_H

#include "macromagic.h"
#include "mesh.h"

typedef struct TerrainChunk {
    Mesh mesh;
} TerrainChunk;

void terrain_create_chunk_mesh(TerrainChunk* chunk);

#endif