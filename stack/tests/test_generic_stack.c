#include <stdio.h>
#ifdef TEST
#include "generic_stack.h"
#include "unity.h"
#else
#include "../../unity/src/unity.h"
#include "../src/generic_stack.h"
#endif
generic_stack_t *stack = NULL;
size_t size = sizeof(int);
void setUp() { stack = generic_stack_create(size); }
void tearDown() { generic_stack_free(stack); }

#define fname(name, type) test_generic_stack_##name##_##type
#define test(name, type) void fname(name, type)()

test(push, int) {
    int num = 1 << 20;
    int err = generic_stack_push(stack, &num);
    TEST_ASSERT_EQUAL(0, err);
    TEST_ASSERT_EQUAL(1, stack->size);
    TEST_ASSERT_EQUAL(num, *(int *)generic_stack_top(stack, &err));
    TEST_ASSERT_EQUAL(0, err);
    TEST_ASSERT_EQUAL(EFAULT, generic_stack_push(NULL, &num));
    TEST_ASSERT_EQUAL(EINVAL, generic_stack_push(stack, NULL));
}

test(push, str) {
    char *str = "i hate the antichrist";
    int err = generic_stack_push(stack, &str);
    TEST_ASSERT_EQUAL(0, err);
    TEST_ASSERT_EQUAL_STRING(str, *(char **)generic_stack_top(stack, &err));
    TEST_ASSERT_EQUAL(0, err);
}

test(top, int) {
    int err;
    generic_stack_top(NULL, &err);
    TEST_ASSERT_EQUAL(EFAULT, err);
    generic_stack_top(stack, &err);
    TEST_ASSERT_EQUAL(EINVAL, err);
    int num = 30;
    generic_stack_push(stack, &num);
    num = 50;
    generic_stack_push(stack, &num);
    TEST_ASSERT_EQUAL(50, *(int *)generic_stack_top(stack, NULL));
    generic_stack_pop(stack);
    TEST_ASSERT_EQUAL(30, *(int *)generic_stack_top(stack, NULL));
}

test(top, str) {
    char *str = "corinthians";
    generic_stack_push(stack, &str);
    str = "flamengo";
    generic_stack_push(stack, &str);
    TEST_ASSERT_EQUAL_STRING("flamengo",
                             *(char **)generic_stack_top(stack, NULL));
    generic_stack_pop(stack);
    TEST_ASSERT_EQUAL_STRING("corinthians",
                             *(char **)generic_stack_top(stack, NULL));
}

test(pop, int) {
    TEST_ASSERT_EQUAL(EFAULT, generic_stack_pop(NULL));
    TEST_ASSERT_EQUAL(EINVAL, generic_stack_pop(stack));
    int num = 30;
    generic_stack_push(stack, &num);
    num = 50;
    generic_stack_push(stack, &num);
    generic_stack_pop(stack);
    TEST_ASSERT_EQUAL(30, *(int *)generic_stack_top(stack, NULL));
}

test(pop, str) {
    char *str = "str";
    generic_stack_push(stack, &str);
    str = "treco";
    generic_stack_push(stack, &str);
    generic_stack_pop(stack);
    TEST_ASSERT_EQUAL_STRING("str", *(char **)generic_stack_top(stack, NULL));
}

test(order, str) {
    char *list[11];
    char fmt[] = "%d %04d";
    for (int i = 0; i <= 10; ++i) {
        int len = snprintf(NULL, 0, fmt, i, i * 10) + 1;
        list[i] = __builtin_malloc(len);
        sprintf(list[i], fmt, i, i * 10);
        generic_stack_push(stack, &list[i]);
    }

    for (int i = 10; i >= 0; --i) {
        TEST_ASSERT_EQUAL_STRING(list[i],
                                 *(char **)generic_stack_top(stack, NULL));
        generic_stack_pop(stack);
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
