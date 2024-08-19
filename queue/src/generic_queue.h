#ifndef GENERIC_QUEUE_H
#define GENERIC_QUEUE_H
#include <errno.h>
#include <stddef.h>

typedef struct _generic_queue_node {
    void *val;
    struct _generic_queue_node *next;
} _generic_queue_node;

typedef struct generic_queue_t {
    int size;
    size_t elem_size;
    struct _generic_queue_node *tail;
    struct _generic_queue_node *head;
} generic_queue_t;

static _generic_queue_node *_generic_queue_node_create(const void *val,
                                                       size_t size) {
    if (val == NULL)
        return NULL;
    _generic_queue_node *node =
        (_generic_queue_node *)__builtin_malloc(sizeof(_generic_queue_node));
    if (node == NULL)
        return NULL;
    node->val = __builtin_malloc(size);
    if (node->val == NULL) {
        __builtin_free(node);
        node = NULL;
        return NULL;
    }
    __builtin_memmove(node->val, val, size);
    node->next = NULL;
    return node;
}

static void _generic_queue_node_free(_generic_queue_node *node) {
    if (node == NULL)
        return;
    if (node->val != NULL) {
        __builtin_free(node->val);
        node->val = NULL;
    }
    __builtin_free(node);
    node = NULL;
}

static generic_queue_t *generic_queue_create(size_t elem_size) {
    generic_queue_t *queue =
        (generic_queue_t *)__builtin_malloc(sizeof(generic_queue_t));
    if (queue == NULL)
        return NULL;
    queue->size = 0;
    queue->elem_size = elem_size;
    queue->tail = queue->head = NULL;
    return queue;
}

static int generic_queue_push(generic_queue_t *queue, const void *val) {
    if (queue == NULL)
        return EFAULT;
    if (val == NULL)
        return EINVAL;
    _generic_queue_node *x = _generic_queue_node_create(val, queue->elem_size);
    if (x == NULL)
        return ENOMEM;
    if (queue->tail != NULL)
        queue->tail->next = x;
    queue->tail = x;
    if (queue->head == NULL)
        queue->head = x;
    queue->size++;
    return 0;
}

static int generic_queue_empty(const generic_queue_t *queue) {
    if (queue == NULL)
        return EFAULT;
    return queue->size == 0 || queue->head == NULL || queue->tail == NULL;
}

static int generic_queue_pop(generic_queue_t *queue) {
    if (queue == NULL)
        return EFAULT;
    if (generic_queue_empty(queue))
        return EINVAL;

    _generic_queue_node *node = queue->head;
    queue->head = node->next;
    _generic_queue_node_free(node);
    if (queue->head == NULL)
        queue->tail = NULL;
    queue->size--;
    return 0;
}

static void *generic_queue_top(const generic_queue_t *queue, int *err) {
    if (queue == NULL) {
        if (err != NULL)
            *err = EFAULT;
        return NULL;
    }
    if (generic_queue_empty(queue)) {
        if (err != NULL)
            *err = EINVAL;
        return NULL;
    }
    if (err != NULL)
        *err = 0;
    return queue->head->val;
}

static void generic_queue_free(generic_queue_t *queue) {
    while (!generic_queue_empty(queue))
        generic_queue_pop(queue);
    __builtin_free(queue);
    queue = NULL;
}
#endif // !GENERIC_QUEUE_H
