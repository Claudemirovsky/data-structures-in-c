#ifdef TEST
#include "queue.h"
#include "unity.h"
#else
#include "../../unity/src/unity.h"
#include "../src/queue.h"
#endif
queue_t *queue = NULL;
void setUp() { queue = queue_create(); }
void tearDown() { queue_free(queue); }

void test_queue_push() {
    int err = queue_push(queue, 1 << 20);
    TEST_ASSERT_EQUAL_INT(0, err);
    TEST_ASSERT_EQUAL_INT(1, queue->size);
    TEST_ASSERT_EQUAL_INT(1 << 20, queue_top(queue, &err));
    TEST_ASSERT_EQUAL_INT(0, err);
    err = queue_push(NULL, 2);
    TEST_ASSERT_EQUAL_INT(EFAULT, err);
}

void test_queue_top() {
    int err;
    queue_top(queue, &err);
    TEST_ASSERT_EQUAL_INT(EINVAL, err);
    queue_top(NULL, &err);
    TEST_ASSERT_EQUAL_INT(EFAULT, err);
    queue_push(queue, 1 << 12);
    queue_push(queue, 1 << 15);
    TEST_ASSERT_EQUAL_INT(1 << 12, queue_top(queue, NULL));
    queue_pop(queue);
    TEST_ASSERT_EQUAL_INT(1 << 15, queue_top(queue, NULL));
}

void test_queue_pop() {
    TEST_ASSERT_EQUAL(EFAULT, queue_pop(NULL));
    TEST_ASSERT_EQUAL(EINVAL, queue_pop(queue));
    queue_push(queue, 500);
    queue_push(queue, 1200);
    TEST_ASSERT_EQUAL(0, queue_pop(queue));
    TEST_ASSERT_EQUAL(1, queue->size);
    TEST_ASSERT_EQUAL(1200, queue_top(queue, NULL));
}

void test_queue_order() {
    for (int i = 0; i <= 10; ++i)
        queue_push(queue, i);

    for (int i = 0; i <= 10; ++i) {
        TEST_ASSERT_EQUAL(i, queue_top(queue, NULL));
        queue_pop(queue);
    }
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_queue_push);
    RUN_TEST(test_queue_top);
    RUN_TEST(test_queue_pop);
    RUN_TEST(test_queue_order);

    UNITY_END();

    return 0;
}
