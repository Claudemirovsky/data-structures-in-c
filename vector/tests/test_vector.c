#ifdef TEST
#include "unity.h"
#include "vector.h"
#else
#include "../../unity/src/unity.h"
#include "../src/vector.h"
#endif
vector_t *vector = NULL;
void setUp() { vector = vector_create(); }
void tearDown() { vector_free(vector); }

void test_vector_push() {
    int err = vector_push(vector, 1 << 20);
    TEST_ASSERT_EQUAL_INT(0, err);
    TEST_ASSERT_EQUAL_INT(1, vector->size);
    TEST_ASSERT_EQUAL_INT(1 << 20, vector_back(vector, &err));
    TEST_ASSERT_EQUAL_INT(0, err);
    err = vector_push(NULL, 2);
    TEST_ASSERT_EQUAL_INT(EFAULT, err);
}

void test_vector_back() {
    int err;
    vector_back(vector, &err);
    TEST_ASSERT_EQUAL_INT(EINVAL, err);
    vector_back(NULL, &err);
    TEST_ASSERT_EQUAL_INT(EFAULT, err);
    vector_push(vector, 1 << 12);
    TEST_ASSERT_EQUAL_INT(1 << 12, vector_back(vector, NULL));
}

void test_vector_pop() {
    TEST_ASSERT_EQUAL(EFAULT, vector_pop(NULL));
    TEST_ASSERT_EQUAL(EINVAL, vector_pop(vector));
    vector_push(vector, 500);
    vector_push(vector, 1200);
    TEST_ASSERT_EQUAL(0, vector_pop(vector));
    TEST_ASSERT_EQUAL(1, vector->size);
    TEST_ASSERT_EQUAL(500, vector_back(vector, NULL));
}

void test_vector_order() {
    for (int i = 0; i <= 300; ++i)
        vector_push(vector, i);

    for (int i = 300; i >= 0; --i) {
        TEST_ASSERT_EQUAL(i, vector_back(vector, NULL));
        vector_pop(vector);
    }

    TEST_ASSERT_EQUAL_INT(512, vector->capacity);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_vector_push);
    RUN_TEST(test_vector_back);
    RUN_TEST(test_vector_pop);
    RUN_TEST(test_vector_order);

    UNITY_END();

    return 0;
}
