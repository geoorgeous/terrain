#ifndef MESH_H
#define MESH_H

#include <stdlib.h>

#include "gl.h"

typedef struct Mesh {
    GLuint glVao;
    GLuint glVbo;
    GLuint glIbo;
	unsigned int numElements;
} Mesh;

typedef struct MeshVertexAttribute {
    int count;
    GLenum glType;
    int bIntegerStorage;
    int bNormalised;
} MeshVertexAttribute;

typedef struct MeshData {
    MeshVertexAttribute* vertexAttributes;
    int numVertexAttributes;
	float* vertices;
	int numVertices;
	unsigned int* indices;
	int numIndices;
} MeshData;

void mesh_allocate_mesh_data(MeshData* meshData);

void mesh_free_mesh_data(MeshData* meshData);

void mesh_create(Mesh* out, const MeshData* meshData);

void mesh_destroy(Mesh* out);

void mesh_draw_indexed(const Mesh* mesh);

void mesh_draw_unindexed(const Mesh* mesh);

#endif