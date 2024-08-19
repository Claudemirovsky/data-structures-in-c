#ifdef TEST
#include "stack.h"
#include "unity.h"
#else
#include "../../unity/src/unity.h"
#include "../src/stack.h"
#endif
stack_t *stack = NULL;
void setUp() { stack = stack_create(); }
void tearDown() { stack_free(stack); }

void test_stack_push() {
    int err = stack_push(stack, 1 << 20);
    TEST_ASSERT_EQUAL_INT(0, err);
    TEST_ASSERT_EQUAL_INT(1, stack->size);
    TEST_ASSERT_EQUAL_INT(1 << 20, stack_top(stack, &err));
    TEST_ASSERT_EQUAL_INT(0, err);
    err = stack_push(NULL, 2);
    TEST_ASSERT_EQUAL_INT(EFAULT, err);
}

void test_stack_top() {
    int err;
    stack_top(stack, &err);
    TEST_ASSERT_EQUAL_INT(EINVAL, err);
    stack_top(NULL, &err);
    TEST_ASSERT_EQUAL_INT(EFAULT, err);
    stack_push(stack, 1 << 12);
    TEST_ASSERT_EQUAL_INT(1 << 12, stack_top(stack, NULL));
}

void test_stack_pop() {
    TEST_ASSERT_EQUAL(EFAULT, stack_pop(NULL));
    TEST_ASSERT_EQUAL(EINVAL, stack_pop(stack));
    stack_push(stack, 500);
    stack_push(stack, 1200);
    TEST_ASSERT_EQUAL(0, stack_pop(stack));
    TEST_ASSERT_EQUAL(1, stack->size);
    TEST_ASSERT_EQUAL(500, stack_top(stack, NULL));
}

void test_stack_order() {
    for (int i = 0; i <= 10; ++i)
        stack_push(stack, i);

    for (int i = 10; i >= 0; --i) {
        TEST_ASSERT_EQUAL(i, stack_top(stack, NULL));
        stack_pop(stack);
    }
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_stack_push);
    RUN_TEST(test_stack_top);
    RUN_TEST(test_stack_pop);
    RUN_TEST(test_stack_order);

    UNITY_END();

    return 0;
}
