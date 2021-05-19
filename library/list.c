#include "list.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct list {
    size_t capacity;
    size_t size;
    void **data;
    free_func_t freer;
} list_t;

list_t *list_init(size_t initial_size, free_func_t freer) {
    list_t *list = malloc(sizeof(list_t));
    assert(list != NULL);
    list->size = 0;
    if (initial_size == 0) {
        initial_size = 1;
    }
    list->capacity = initial_size;
    list->data = malloc(sizeof(void *) * initial_size);
    list->freer = freer;
    return list;
}

void list_free_wo_data(list_t *list) {
    free(list->data);
    free(list);
}

void list_free(list_t *list) {
    free_func_t freer = list->freer;
    for (size_t i = 0; i < list->size; i++) {
        freer(list->data[i]);
    }
    free(list->data);
    free(list);
}

size_t list_size(list_t *list) {
    return list->size;
}

void *list_get(list_t *list, size_t index) {
    assert(index < list->size);
    assert(list->data[index] != NULL);
    return (list->data[index]);
}

void list_add(list_t *list, void *value) {
    assert(value != NULL);
    if (list->size >= list->capacity) {
        list->data = (void **) realloc(list->data, (list->capacity) * 2 * sizeof(void *));
        list->capacity *= 2;
    }
    list->data[list->size] = value;
    list->size++;
}

void *list_remove(list_t *list, size_t index){
    void* temp = list_get(list, index);
    for (size_t i = index + 1; i < list->size; i++) {
        list->data[i - 1] = list->data[i];
    }
    list->size--;
    return temp;
}
