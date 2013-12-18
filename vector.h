#include "binary.h"

typedef struct vector_impl {
	size_t size;
	size_t capacity;
	void *entries[];
} vector_impl;

typedef struct vector {
	vector_impl *v;
} vector;

vector vector_new_capacity(size_t capacity) {
	vector_impl *v = malloc(sizeof(vector_impl) + sizeof(void *) * capacity);
	v->size = 0;
	v->capacity = capacity;

	return (vector){v};
}

void vector_free(vector *v) {
	free(v->v);
}

void vector_reserve(vector *v, size_t n) {
	if (v->v->size + n <= v->v->capacity) {
		return;
	}
	size_t new_capacity = min_power_of_two(v->v->size + n);
	v->v = realloc(
		v->v, sizeof(vector_impl) + sizeof(void *) * new_capacity
	);
	v->v->capacity = new_capacity;
}

void vector_append(vector *v, void *value) {
	vector_reserve(v, 1);
	v->v->entries[v->v->size] = value;
	v->v->size++;
}

void *vector_get(vector *v, size_t index) {
	return v->v->entries[index];
}

size_t vector_size(vector *v) {
	return v->v->size;
}
