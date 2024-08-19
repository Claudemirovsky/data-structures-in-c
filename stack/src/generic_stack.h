#ifndef GENERIC_STACK_H
#define GENERIC_STACK_H

#include <errno.h>
#include <stddef.h>

typedef struct _generic_stack_node {
    void *val;
    struct _generic_stack_node *prev;
} _generic_stack_node;

typedef struct generic_stack_t {
    int size;
    size_t elem_size;
    _generic_stack_node *top_node;
} generic_stack_t;

static _generic_stack_node *
_generic_stack_node_create(_generic_stack_node *prev, const void *val,
                           size_t size) {
    if (val == NULL)
        return NULL;
    _generic_stack_node *node =
        (_generic_stack_node *)__builtin_malloc(sizeof(_generic_stack_node));
    if (node == NULL)
        return NULL;
    node->val = __builtin_malloc(size);
    if (node->val == NULL) {
        __builtin_free(node);
        return NULL;
    }
    __builtin_memmove(node->val, val, size);
    node->prev = prev;
    return node;
}

static void _generic_stack_node_free(_generic_stack_node *node) {
    if (node == NULL)
        return;
    if (node->val != NULL) {
        __builtin_free(node->val);
        node->val = NULL;
    }
    __builtin_free(node);
    node = NULL;
}

static generic_stack_t *generic_stack_create(size_t elem_size) {
    generic_stack_t *res =
        (generic_stack_t *)__builtin_malloc(sizeof(generic_stack_t));
    if (res == NULL)
        return NULL;
    res->size = 0;
    res->top_node = NULL;
    res->elem_size = elem_size;
    return res;
}

static int generic_stack_push(generic_stack_t *obj, const void *val) {
    if (obj == NULL)
        return EFAULT;
    if (val == NULL)
        return EINVAL;
    _generic_stack_node *node =
        _generic_stack_node_create(obj->top_node, val, obj->elem_size);
    if (node == NULL)
        return ENOMEM;
    obj->top_node = node;
    obj->size++;
    return 0;
}

static int generic_stack_empty(const generic_stack_t *obj) {
    if (obj == NULL)
        return EFAULT;
    return obj->size == 0 || obj->top_node == NULL;
}

static int generic_stack_pop(generic_stack_t *obj) {
    if (obj == NULL)
        return EFAULT;
    if (generic_stack_empty(obj))
        return EINVAL;

    _generic_stack_node *top = obj->top_node;
    obj->top_node = top->prev;
    _generic_stack_node_free(top);
    obj->size--;
    return 0;
}

static void *generic_stack_top(const generic_stack_t *obj, int *err) {
    if (obj == NULL) {
        if (err != NULL)
            *err = EFAULT;
        return NULL;
    }
    if (generic_stack_empty(obj)) {
        if (err != NULL)
            *err = EINVAL;
        return NULL;
    }
    if (err != NULL)
        *err = 0;
    return obj->top_node->val;
}

static void generic_stack_free(generic_stack_t *obj) {
    if (obj == NULL)
        return;
    while (!generic_stack_empty(obj))
        generic_stack_pop(obj);
    __builtin_free(obj);
    obj = NULL;
}

#endif // GENERIC_generic_stack_H
