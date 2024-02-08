#ifndef TEXTURE_H
#define TEXTURE_H

#include "gl.h"
#include "stb_image.h"

typedef struct Image {
	int width;
	int height;
	int numChannels;
	unsigned char* pixels;
} Image;

typedef struct Texture {
	GLuint glHandle;
} Texture;

void image_load_file(Image* out, const char* filepath)
{
	out->pixels = stbi_load(filepath, &out->width, &out->height, &out->numChannels, 0);
}

void image_free(Image* out)
{
	out->width = out->height = out->numChannels = 0;
	stbi_image_free(out->pixels);
}

void texture_create(Texture* out)
{
	glGenTextures(1, &out->glHandle);
	glBindTexture(GL_TEXTURE_2D, out->glHandle);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void texture_destroy(Texture* out)
{
	glDeleteTextures(1, &out->glHandle);
}

void texture_set(Texture* out, const Image* image)
{
	glBindTexture(GL_TEXTURE_2D, out->glHandle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->width, image->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image->pixels);
	glGenerateMipmap(GL_TEXTURE_2D);
}

void texture_array_create(Texture* out, GLsizei width, GLsizei height, GLsizei depth)
{
	glGenTextures(1, &out->glHandle);
	glBindTexture(GL_TEXTURE_2D_ARRAY, out->glHandle);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGB, width, height, depth, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
}

void texture_array_set(Texture* out, const Image* image, GLsizei index)
{
	glBindTexture(GL_TEXTURE_2D_ARRAY, out->glHandle);
	glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, index, image->width, image->height, 1, GL_RGB, GL_UNSIGNED_BYTE, image->pixels);
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
}

#endif