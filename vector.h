#ifndef VECTOR_H
#define VECTOR_H

#include "stdlib.h"
#include "string.h"

typedef struct Vector {
	int num; // number of used elements
	int typeSize; // size of element type in bytes
	int capacity; // number of elements vector has allocated space for
	void* data; // pointer to array
} Vector;

void vector_set_capacity(Vector* out, int capactiy)
{
	if (out->capacity == capactiy)
		return;

	out->data = realloc(out->data, out->typeSize * capactiy);
	out->capacity = capactiy;
}

void vector_init_capacity(Vector* out, int typeSize, int capacity)
{
	out->num = 0;
	out->capacity = 0;
	out->typeSize = typeSize;
	out->data = NULL;
	vector_set_capacity(out, capacity);
}

void vector_init(Vector* out, int typeSize)
{
	vector_init_capacity(out, typeSize, 1);
}

void vector_destroy(Vector* out)
{
	out->num = 0;
	out->typeSize = 0;
	out->capacity = 0;
	free(out->data);
}

void* vector_get(const Vector* vector, int index)
{
	if (index >= vector->num)
		return (void*)0;
	return (void*)((char*)vector->data + (vector->typeSize * index));
}

void* vector_push_back(Vector* out)
{
	if (out->num == out->capacity)
		vector_set_capacity(out, out->capacity * 2);
	out->num++;
	return vector_get(out, out->num - 1);
}

void vector_swap(Vector* out, int i0, int i1)
{
	char temp[out->typeSize];
	memcpy((void*)temp, vector_get(out, i0), out->typeSize);
	memmove(vector_get(out, i0), vector_get(out, i1), out->typeSize);
	memcpy(vector_get(out, i1), (void*)temp, out->typeSize);
}

void vector_swap_back(Vector* out, int index)
{
	vector_swap(out, index, out->num - 1);
}

void vector_remove(Vector* out, int index)
{
	vector_swap_back(out, index);
	out->num--;
}

#endif