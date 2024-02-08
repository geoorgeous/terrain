#ifndef GLUTILS_H
#define GLUTILS_H

#include "logging.h"
#include "gl.h"

size_t gut_get_type_size(GLenum type);

int gut_create_shader(GLenum type, const GLchar* source, GLuint* shader);

int gut_create_shader_program(const GLchar* vertexSource, const GLchar* fragmentSource, GLuint* program);

void gut_set_shader_uniform(GLuint glProgram, GLint uniformType, const GLchar* uniformName, const void* data);

void gut_create_buffer(GLuint* glHandle, size_t size, void* data, GLenum draw);

GLuint gut_create_texture();

#endif