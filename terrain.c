#include "terrain.h"

void terrain_create_chunk_mesh(TerrainChunk* chunk) 
{
    const int size = 16;

    MeshData data;

    MeshVertexAttribute vertexAttributes[1];
    vertexAttributes[0].count = 3;
    vertexAttributes[0].glType = GL_FLOAT;
    vertexAttributes[0].bIntegerStorage = FALSE;
    vertexAttributes[0].bNormalised = FALSE;
    
    data.vertexAttributes = vertexAttributes;
    data.numVertexAttributes = 1;
    data.numVertices = (size + 1) * (size + 1);
    data.numIndices = size * size * 6;
    mesh_allocate_mesh_data(&data);

    for (int vx = 0; vx < size; ++vx)
        for (int vy = 0; vy < size; ++vy)
        {
            float vposx = (float)vx - 0.5f;
            float vposy = 0.0f;
            float vposz = (float)vx - 0.5f;
            float vnorx = 0.0f;
            float vnory = 1.0f;
            float vnorz = 0.0f;
            float vtexu = 0.0f;
            float vtexv = 0.0f;

            float* vv = data.vertices + (size * vx + vy) * 3;
            *vv = vposx;
            *vv++ = vposy;
            *vv++ = vposz;
            *vv++ = vnorx;
            *vv++ = vnory;
            *vv++ = vnorz;
            *vv++ = vtexu;
            *vv = vtexv;
        }

    for (int vx = 0; vx < size; ++vx)
        for (int vy = 0; vy < size; ++vy)
        {
            int v0 = vx * size + vy;
            int v1 = v0 + 1;
            int v2 = v0 + size;
            int v3 = v2 + 1;

            unsigned int* iv = data.indices + (size * vx + vy) * 6;
            *iv = v0;
            *iv++ = v1;
            *iv++ = v2;
            *iv++ = v2;
            *iv++ = v1;
            *iv++ = v3;
        }

    mesh_create(&chunk->mesh, &data);

    mesh_free_mesh_data(&data);
}