#ifndef QUEUE_H
#define QUEUE_H
#include <errno.h>
#include <stddef.h>

typedef struct _queue_node {
    int val;
    struct _queue_node *next;
} _queue_node;

typedef struct queue_t {
    int size;
    struct _queue_node *tail;
    struct _queue_node *head;
} queue_t;

static _queue_node *_queue_node_create(int val) {
    _queue_node *node = (_queue_node *)__builtin_malloc(sizeof(_queue_node));
    if (node == NULL)
        return NULL;
    node->val = val;
    node->next = NULL;
    return node;
}

static queue_t *queue_create() {
    queue_t *queue = (queue_t *)__builtin_malloc(sizeof(queue_t));
    if (queue == NULL)
        return NULL;
    queue->size = 0;
    queue->tail = queue->head = NULL;
    return queue;
}

static int queue_push(queue_t *queue, int val) {
    if (queue == NULL)
        return EFAULT;
    _queue_node *x = _queue_node_create(val);
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

static int queue_empty(const queue_t *queue) {
    if (queue == NULL)
        return EFAULT;
    return queue->size == 0 || queue->head == NULL || queue->tail == NULL;
}

static int queue_pop(queue_t *queue) {
    if (queue == NULL)
        return EFAULT;
    if (queue_empty(queue))
        return EINVAL;

    _queue_node *node = queue->head;
    queue->head = node->next;
    __builtin_free(node);
    node = NULL;
    if (queue->head == NULL)
        queue->tail = NULL;
    queue->size--;
    return 0;
}

static int queue_top(const queue_t *queue, int *err) {
    if (queue == NULL) {
        *err = EFAULT;
        return -1;
    }
    if (queue_empty(queue)) {
        *err = EINVAL;
        return -1;
    }
    return queue->head->val;
}

static void queue_free(queue_t *queue) {
    while (!queue_empty(queue))
        queue_pop(queue);
    __builtin_free(queue);
    queue = NULL;
}
#endif // !QUEUE_H
