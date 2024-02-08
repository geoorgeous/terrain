#include "glutils.h"
#include "macromagic.h"
#include "mesh.h"

size_t calculate_vertex_size(const MeshVertexAttribute* vertexAttributes, int numVertexAttributes)
{
    int size = 0;
    for (int i = 0; i < numVertexAttributes; ++i)
        size += vertexAttributes[i].count * gut_get_type_size(vertexAttributes[i].glType);
    return size;
}

void mesh_allocate_mesh_data(MeshData* meshData)
{
    size_t vertexSize = calculate_vertex_size(meshData->vertexAttributes, meshData->numVertexAttributes);
	meshData->vertices = (float*)malloc(vertexSize * meshData->numVertices);
	meshData->indices = meshData->numIndices == 0 ? NULL : (unsigned int*)malloc(sizeof(unsigned int) * meshData->numIndices);
}

void mesh_free_mesh_data(MeshData* meshData)
{
	free(meshData->vertices);
	free(meshData->indices);
}

void mesh_create(Mesh* out, const MeshData* meshData)
{
    glGenVertexArrays(1, &out->glVao);
    glBindVertexArray(out->glVao);

    const size_t vertexSize = calculate_vertex_size(meshData->vertexAttributes, meshData->numVertexAttributes);

    gut_create_buffer(&out->glVbo, vertexSize * meshData->numVertices, meshData->vertices, GL_STATIC_DRAW);

    if (meshData->numIndices)
    {
        gut_create_buffer(&out->glIbo, sizeof(unsigned int) * meshData->numIndices, meshData->indices, GL_STATIC_DRAW);
	    out->numElements = meshData->numIndices;
    }
    else
    {
	    out->numElements = meshData->numVertices;
    }

    size_t offset = 0;
    for (GLuint a = 0; a < meshData->numVertexAttributes; ++a)
    {
        if (meshData->vertexAttributes[a].bIntegerStorage)
            glVertexAttribIPointer(a,
                (GLint)meshData->vertexAttributes[a].count,
                meshData->vertexAttributes[a].glType,
                (GLsizei)vertexSize,
                (void*)offset);
        else
            glVertexAttribPointer(a,
                (GLint)meshData->vertexAttributes[a].count,
                meshData->vertexAttributes[a].glType,
                meshData->vertexAttributes[a].bNormalised ? GL_TRUE : GL_FALSE,
                (GLsizei)vertexSize,
                (void*)offset);
        glEnableVertexAttribArray(a);
        offset += meshData->vertexAttributes[a].count * gut_get_type_size(meshData->vertexAttributes[a].glType);
    }
}

void mesh_destroy(Mesh* out)
{
	glDeleteBuffers(1, &out->glIbo);
	glDeleteBuffers(1, &out->glVbo);
	glDeleteVertexArrays(1, &out->glVao);
}

void mesh_draw_indexed(const Mesh* mesh)
{
    glBindVertexArray(mesh->glVao);
    glDrawElements(GL_TRIANGLES, mesh->numElements, GL_UNSIGNED_INT, 0);
}

void mesh_draw_unindexed(const Mesh* mesh)
{
    glBindVertexArray(mesh->glVao);
    glDrawArrays(GL_POINTS, 0, mesh->numElements);
}

// void mesh_generate_cube(Mesh * out)
// {
//     const float hs = 0.5f;

//     MeshData data;

//     MeshVertexAttribute vertexAttributes[3];
//     vertexAttributes[0].count = 3; // position
//     vertexAttributes[0].glType = GL_FLOAT;
//     vertexAttributes[0].bIntegerStorage = FALSE;
//     vertexAttributes[0].bNormalised = FALSE;
//     vertexAttributes[1].count = 3; // normal
//     vertexAttributes[1].glType = GL_FLOAT;
//     vertexAttributes[1].bIntegerStorage = FALSE;
//     vertexAttributes[1].bNormalised = FALSE;
//     vertexAttributes[2].count = 2; // tex coords
//     vertexAttributes[2].glType = GL_FLOAT;
//     vertexAttributes[2].bIntegerStorage = FALSE;
//     vertexAttributes[2].bNormalised = FALSE;

//     data.vertexAttributes = vertexAttributes;
//     data.numVertexAttributes = 3;
//     data.numVertices = 24;
//     data.numIndices = 36;
//     mesh_allocate_mesh_data(&out);

//     mesh_create(out, &data);

//     mesh_free_mesh_data(&out);
// }