#ifndef GENERIC_VECTOR_H
#define GENERIC_VECTOR_H
#include <errno.h>
#include <stddef.h>

typedef struct {
    int elem_size;
    int size;
    int capacity;
    void *array;
} generic_vector_t;

static generic_vector_t *generic_vector_create(size_t elem_size) {
    generic_vector_t *vector =
        (generic_vector_t *)__builtin_malloc(sizeof(generic_vector_t));
    if (vector == NULL)
        return NULL;
    vector->capacity = vector->size = 0;
    vector->array = NULL;
    vector->elem_size = elem_size;
    return vector;
}

static int generic_vector_reserve(generic_vector_t *vector, int size) {
    if (vector == NULL)
        return -EFAULT;
    if (size <= 0 || size < vector->size)
        return -EINVAL;
    if (size <= vector->capacity)
        return 0;

    void *ptr = NULL;
    if (vector->array == NULL) {
        ptr = (void *)__builtin_malloc(vector->elem_size * size);
    } else {
        ptr =
            (void *)__builtin_realloc(vector->array, vector->elem_size * size);
    }
    if (ptr == NULL)
        return -ENOMEM;
    vector->array = ptr;
    vector->capacity = size;
    ptr = NULL;
    return 0;
}

static void generic_vector_free(generic_vector_t *vector) {
    if (vector == NULL)
        return;
    if (vector->array != NULL) {
        __builtin_free(vector->array);
        vector->array = NULL;
    }
    __builtin_free(vector);
    vector = NULL;
}

static int generic_vector_push(generic_vector_t *vector, const void *val) {
    if (vector == NULL)
        return -EFAULT;
    if (val == NULL)
        return -EINVAL;
    if (vector->capacity < (vector->size + 1)) {
        if (vector->capacity == 0 || vector->array == NULL) {
            vector->array = (void *)__builtin_malloc(vector->elem_size);
            if (vector->array == NULL)
                return -ENOMEM;
            vector->capacity = 1;
        } else {
            void *ptr = (void *)__builtin_realloc(
                vector->array, vector->elem_size * (vector->capacity << 1));
            if (ptr == NULL)
                return -ENOMEM;
            vector->capacity <<= 1;
            vector->array = ptr;
        }
    }

    __builtin_memmove((char *)vector->array +
                          (vector->size++ * vector->elem_size),
                      val, vector->elem_size);
    return 0;
}

static int generic_vector_empty(const generic_vector_t *vector) {
    if (vector == NULL)
        return -EFAULT;
    return vector->size == 0 || vector->array == NULL;
}

static void *generic_vector_back(const generic_vector_t *vector, int *err) {
    if (vector == NULL) {
        if (err != NULL)
            *err = -EFAULT;
        return NULL;
    }
    if (generic_vector_empty(vector)) {
        if (err != NULL)
            *err = -EINVAL;
        return NULL;
    }

    if (err != NULL)
        *err = 0;
    size_t offset = vector->elem_size * (vector->size - 1);
    return (void *)((char *)vector->array + offset);
}

static int generic_vector_pop(generic_vector_t *vector) {
    if (vector == NULL)
        return -EFAULT;
    if (generic_vector_empty(vector))
        return -EINVAL;
    vector->size--;
    return 0;
}

static void *generic_vector_get(const generic_vector_t *vector, int index,
                                int *err) {
    if (index < 0 || index >= vector->size) {
        if (err != NULL)
            *err = -EINVAL;
        return NULL;
    }
    size_t offset = vector->elem_size * index;
    return (void *)((char *)vector->array + offset);
}
#endif // !GENERIC_VECTOR_H
