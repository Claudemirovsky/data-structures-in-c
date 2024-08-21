#ifndef VECTOR_H
#define VECTOR_H
#include <errno.h>
#include <stddef.h>

typedef struct vector_t {
    int *array;
    int size;
    int capacity;
} vector_t;

static vector_t *vector_create() {
    vector_t *vector = (vector_t *)__builtin_malloc(sizeof(vector_t));
    if (vector == NULL)
        return NULL;
    vector->capacity = vector->size = 0;
    vector->array = NULL;
    return vector;
}

static void vector_free(vector_t *vector) {
    if (vector == NULL)
        return;
    if (vector->array != NULL) {
        __builtin_free(vector->array);
        vector->array = NULL;
    }
    __builtin_free(vector);
    vector = NULL;
}

static int vector_push(vector_t *vector, int val) {
    if (vector == NULL)
        return EFAULT;
    if (vector->capacity < (vector->size + 1)) {
        if (vector->capacity == 0 || vector->array == NULL) {
            vector->array = (int *)__builtin_malloc(sizeof(int) * 1);
            if (vector->array == NULL)
                return ENOMEM;
            vector->capacity = 1;
        } else {
            int *ptr = (int *)__builtin_realloc(
                vector->array, sizeof(int) * (vector->capacity << 1));
            if (ptr == NULL)
                return ENOMEM;
            vector->capacity <<= 1;
            vector->array = ptr;
        }
    }

    vector->array[vector->size++] = val;
    return 0;
}

static int vector_empty(const vector_t *vector) {
    if (vector == NULL)
        return EFAULT;
    return vector->size == 0 || vector->array == NULL;
}

static int vector_back(const vector_t *vector, int *err) {
    if (vector == NULL) {
        if (err != NULL)
            *err = EFAULT;
        return -1;
    }
    if (vector_empty(vector)) {
        if (err != NULL)
            *err = EINVAL;
        return -1;
    }

    if (err != NULL)
        *err = 0;
    return vector->array[vector->size - 1];
}

static int vector_pop(vector_t *vector) {
    if (vector == NULL)
        return EFAULT;
    if (vector_empty(vector))
        return EINVAL;
    vector->size--;
    return 0;
}
#endif // !VECTOR_H
