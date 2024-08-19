#ifndef STACK_H
#define STACK_H

#include <errno.h>
#include <stddef.h>
typedef struct _stack_node {
    int val;
    struct _stack_node *prev;
} _stack_node;

typedef struct stack_t {
    int size;
    _stack_node *top_node;
} stack_t;

static _stack_node *stack_node_create(_stack_node *prev, int val) {
    _stack_node *node = (_stack_node *)__builtin_malloc(sizeof(_stack_node));
    if (node == NULL)
        return NULL;
    node->prev = prev;
    node->val = val;
    return node;
}

static stack_t *stack_create() {
    stack_t *res = (stack_t *)__builtin_malloc(sizeof(stack_t));
    if (res == NULL)
        return NULL;
    res->size = 0;
    res->top_node = NULL;
    return res;
}

static int stack_push(stack_t *obj, int val) {
    if (obj == NULL)
        return EFAULT;
    _stack_node *node = stack_node_create(obj->top_node, val);
    if (node == NULL)
        return ENOMEM;
    obj->top_node = node;
    obj->size++;
    return 0;
}

static int stack_empty(const stack_t *obj) {
    if (obj == NULL)
        return EFAULT;
    return obj->size == 0 || obj->top_node == NULL;
}

static int stack_pop(stack_t *obj) {
    if (obj == NULL)
        return EFAULT;
    if (stack_empty(obj))
        return EINVAL;

    _stack_node *top = obj->top_node;
    obj->top_node = top->prev;
    __builtin_free(top);
    top = NULL;
    obj->size--;
    return 0;
}

static int stack_top(const stack_t *obj, int *err) {
    if (obj == NULL) {
        if (err != NULL)
            *err = EFAULT;
        return -1;
    }
    if (stack_empty(obj)) {
        if (err != NULL)
            *err = EINVAL;
        return -1;
    }
    if (err != NULL)
        *err = 0;
    return obj->top_node->val;
}

static void stack_free(stack_t *obj) {
    if (obj == NULL)
        return;
    while (!stack_empty(obj))
        stack_pop(obj);
    __builtin_free(obj);
    obj = NULL;
}

#endif // !STACK_H
