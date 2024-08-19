#include <stdio.h>
#ifdef TEST
#include "generic_queue.h"
#include "unity.h"
#else
#include "../../unity/src/unity.h"
#include "../src/generic_queue.h"
#endif

generic_queue_t *queue = NULL;
size_t size = sizeof(int);
void setUp() { queue = generic_queue_create(size); }
void tearDown() { generic_queue_free(queue); }

#define fname(name, type) test_generic_queue_##name##_##type
#define test(name, type) void fname(name, type)()

test(push, int) {
    int num = 1 << 20;
    int err = generic_queue_push(queue, &num);
    TEST_ASSERT_EQUAL(0, err);
    TEST_ASSERT_EQUAL(1, queue->size);
    TEST_ASSERT_EQUAL(num, *(int *)generic_queue_top(queue, &err));
    TEST_ASSERT_EQUAL(0, err);
    TEST_ASSERT_EQUAL(EFAULT, generic_queue_push(NULL, &num));
    TEST_ASSERT_EQUAL(EINVAL, generic_queue_push(queue, NULL));
}

test(push, str) {
    char *str = "i hate the antichrist";
    int err = generic_queue_push(queue, &str);
    TEST_ASSERT_EQUAL(0, err);
    TEST_ASSERT_EQUAL_STRING(str, *(char **)generic_queue_top(queue, &err));
    TEST_ASSERT_EQUAL(0, err);
}

test(top, int) {
    int err;
    generic_queue_top(NULL, &err);
    TEST_ASSERT_EQUAL(EFAULT, err);
    generic_queue_top(queue, &err);
    TEST_ASSERT_EQUAL(EINVAL, err);
    int num = 30;
    generic_queue_push(queue, &num);
    num = 50;
    generic_queue_push(queue, &num);
    TEST_ASSERT_EQUAL(30, *(int *)generic_queue_top(queue, NULL));
    generic_queue_pop(queue);
    TEST_ASSERT_EQUAL(50, *(int *)generic_queue_top(queue, NULL));
}

test(top, str) {
    char *str = "corinthians";
    generic_queue_push(queue, &str);
    str = "flamengo";
    generic_queue_push(queue, &str);
    TEST_ASSERT_EQUAL_STRING("corinthians",
                             *(char **)generic_queue_top(queue, NULL));
    generic_queue_pop(queue);
    TEST_ASSERT_EQUAL_STRING("flamengo",
                             *(char **)generic_queue_top(queue, NULL));
}

test(pop, int) {
    TEST_ASSERT_EQUAL(EFAULT, generic_queue_pop(NULL));
    TEST_ASSERT_EQUAL(EINVAL, generic_queue_pop(queue));
    int num = 30;
    generic_queue_push(queue, &num);
    num = 50;
    generic_queue_push(queue, &num);
    generic_queue_pop(queue);
    TEST_ASSERT_EQUAL(50, *(int *)generic_queue_top(queue, NULL));
}

test(pop, str) {
    char *str = "str";
    generic_queue_push(queue, &str);
    str = "treco";
    generic_queue_push(queue, &str);
    generic_queue_pop(queue);
    TEST_ASSERT_EQUAL_STRING("treco", *(char **)generic_queue_top(queue, NULL));
}

test(order, str) {
    char *list[11];
    char fmt[] = "%d %04d";
    for (int i = 0; i <= 10; ++i) {
        int len = snprintf(NULL, 0, fmt, i, i * 10) + 1;
        list[i] = __builtin_malloc(len);
        sprintf(list[i], fmt, i, i * 10);
        generic_queue_push(queue, &list[i]);
    }

    for (int i = 0; i <= 10; ++i) {
        TEST_ASSERT_EQUAL_STRING(list[i],
                                 *(char **)generic_queue_top(queue, NULL));
        generic_queue_pop(queue);
        __builtin_free(list[i]);
    }
}

int main(void) {
    UNITY_BEGIN();
    size = sizeof(int);
    RUN_TEST(fname(push, int));
    RUN_TEST(fname(top, int));
    RUN_TEST(fname(pop, int));
    size = sizeof(char **);
    RUN_TEST(fname(push, str));
    RUN_TEST(fname(top, str));
    RUN_TEST(fname(pop, str));
    RUN_TEST(fname(order, str));
    UNITY_END();

    return 0;
}
