#include "vec_list.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct vec_list {
    size_t capacity;
    size_t size;
    vector_t **vectors;
} vec_list_t;

vec_list_t *vec_list_init(size_t initial_size) {
    vec_list_t *list = malloc(sizeof(vec_list_t));
    assert(list != NULL);
    list->capacity = initial_size;
    list->size = 0;
    list->vectors = malloc(sizeof(vector_t *) * initial_size);
    return list;
}

void vec_list_free(vec_list_t *list) {
    for (size_t i = 0; i < list->size; i++) {
        free(list->vectors[i]);
    }
    free(list->vectors);
    free(list);
}

size_t vec_list_size(vec_list_t *list) {
    return list->size;
}

vector_t *vec_list_get(vec_list_t *list, size_t index) {
    assert(index < list->size);
    assert(list->vectors[index] != NULL);
    return (list->vectors[index]);
}

void vec_list_add(vec_list_t *list, vector_t *value) {
    assert(list->size < list->capacity);
    assert(value != NULL);
    list->vectors[list->size] = value;
    list->size = list->size + 1;
}

vector_t *vec_list_remove(vec_list_t *list) {
    assert(list->size != 0);
    list->size = list->size - 1;
    vector_t *v = list->vectors[list->size];
    return v;
}

int vec_list_get_size(vec_list_t *list) {
    return list->size;
}

vector_t ** vec_list_get_vectors(vec_list_t *list) {
    return list->vectors;
}