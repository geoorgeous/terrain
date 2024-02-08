#include <stddef.h>
#include "gl.h"
#include "glutils.h"

size_t gut_get_type_size(GLenum glType)
{
    switch (glType)
    {
        case GL_FLOAT: return sizeof(float);
        case GL_INT: 
        case GL_UNSIGNED_INT: return sizeof(int);
        default: return 0;
    }
}

int gut_create_shader(GLenum type, const GLchar* source, GLuint* shader)
{
    *shader = glCreateShader(type);
    glShaderSource(*shader, 1, &source, NULL);
    glCompileShader(*shader);

    GLint  success;
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        GLchar infoLog[512];
        glGetShaderInfoLog(*shader, 512, NULL, infoLog);
        LOG("Failed to compiler shader: %s", infoLog);
    }

    return success;
}

int gut_create_shader_program(const GLchar* vertexSource, const GLchar* fragmentSource, GLuint* program)
{
    GLuint vertexShader, fragmentShader;

    if(!gut_create_shader(GL_VERTEX_SHADER, vertexSource, &vertexShader) ||
        !gut_create_shader(GL_FRAGMENT_SHADER, fragmentSource, &fragmentShader))
    {
        return 0;
    }

    *program = glCreateProgram();
    glAttachShader(*program, vertexShader);
    glAttachShader(*program, fragmentShader);
    glLinkProgram(*program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLint success;
    glGetProgramiv(*program, GL_LINK_STATUS, &success);
    if(!success)
    {
        GLchar infoLog[512];
        glGetProgramInfoLog(*program, 512, NULL, infoLog);
        LOG("Shader error: %s", infoLog);
    }

    return success;
}

void gut_set_shader_uniform(GLuint glProgram, GLint uniformType, const GLchar* uniformName, const void* data)
{
    GLint location = glGetUniformLocation(glProgram, uniformName);
    if (location == -1)
    {
        LOG("No unfiform found");
        return;
    }

    switch(uniformType)
    {
    case GL_FLOAT_VEC2: glUniform2fv(location, 1, (GLfloat*)data); break;
    case GL_FLOAT_VEC3: glUniform3fv(location, 1, (GLfloat*)data); break;
    case GL_FLOAT_VEC4: glUniform4fv(location, 1, (GLfloat*)data); break;
    case GL_FLOAT_MAT4: glUniformMatrix4fv(location, 1, GL_FALSE, (GLfloat*)data); break;
    default: LOG("Unsupported uniform type."); break;
    }
}

void gut_create_buffer(GLuint* glHandle, size_t size, void* data, GLenum usage)
{
    glGenBuffers(1, glHandle);
    glBindBuffer(GL_ARRAY_BUFFER, *glHandle);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)size, data, usage);
}

GLuint gut_create_texture()
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    // todo: load BMP
    // unsigned char *data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
    // if (data)
    // {
    //     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    //     glGenerateMipmap(GL_TEXTURE_2D);
    // }
    // else
    // {
    //     std::cout << "Failed to load texture" << std::endl;
    // }
    // stbi_image_free(data);
    return texture;
}