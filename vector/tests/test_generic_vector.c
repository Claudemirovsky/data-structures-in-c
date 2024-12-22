#include <stdio.h>
#ifdef TEST
#include "generic_vector.h"
#include "unity.h"
#else
#include "../../unity/src/unity.h"
#include "../src/generic_vector.h"
#endif
generic_vector_t *vector = NULL;
size_t size = sizeof(int);
char *str_list[16];

void setUp() { vector = generic_vector_create(size); }
void tearDown() { generic_vector_free(vector); }

#define fname(name, type) test_generic_vector_##name##_##type
#define test(name, type) void fname(name, type)()

test(push, int) {
    int num = 1 << 20;
    int err = generic_vector_push(vector, &num);
    TEST_ASSERT_EQUAL(0, err);
    TEST_ASSERT_EQUAL(1, vector->size);
    TEST_ASSERT_EQUAL(num, *(int *)generic_vector_back(vector, &err));
    TEST_ASSERT_EQUAL(0, err);
    TEST_ASSERT_EQUAL(-EFAULT, generic_vector_push(NULL, &num));
    TEST_ASSERT_EQUAL(-EINVAL, generic_vector_push(vector, NULL));
}

test(push, str) {
    char *str = "i hate the antichrist";
    int err = generic_vector_push(vector, &str);
    TEST_ASSERT_EQUAL(0, err);
    TEST_ASSERT_EQUAL_STRING(str, *(char **)generic_vector_back(vector, &err));
    TEST_ASSERT_EQUAL(0, err);
}

test(back, int) {
    int err;
    generic_vector_back(NULL, &err);
    TEST_ASSERT_EQUAL(-EFAULT, err);
    generic_vector_back(vector, &err);
    TEST_ASSERT_EQUAL(-EINVAL, err);
    int num = 30;
    generic_vector_push(vector, &num);
    num = 50;
    generic_vector_push(vector, &num);
    TEST_ASSERT_EQUAL(50, *(int *)generic_vector_back(vector, NULL));
    generic_vector_pop(vector);
    TEST_ASSERT_EQUAL(30, *(int *)generic_vector_back(vector, NULL));
}

test(back, str) {
    char *str = "corinthians";
    generic_vector_push(vector, &str);
    str = "flamengo";
    generic_vector_push(vector, &str);
    TEST_ASSERT_EQUAL_STRING("flamengo",
                             *(char **)generic_vector_back(vector, NULL));
    generic_vector_pop(vector);
    TEST_ASSERT_EQUAL_STRING("corinthians",
                             *(char **)generic_vector_back(vector, NULL));
}

test(pop, int) {
    TEST_ASSERT_EQUAL(-EFAULT, generic_vector_pop(NULL));
    TEST_ASSERT_EQUAL(-EINVAL, generic_vector_pop(vector));
    int num = 30;
    generic_vector_push(vector, &num);
    num = 50;
    generic_vector_push(vector, &num);
    generic_vector_pop(vector);
    TEST_ASSERT_EQUAL(30, *(int *)generic_vector_back(vector, NULL));
}

test(pop, str) {
    char *str = "str";
    generic_vector_push(vector, &str);
    str = "treco";
    generic_vector_push(vector, &str);
    generic_vector_pop(vector);
    TEST_ASSERT_EQUAL_STRING("str",
                             *(char **)generic_vector_back(vector, NULL));
}

test(order, int) {
    for (int i = 0; i <= 300; ++i)
        generic_vector_push(vector, &i);

    for (int i = 300; i >= 0; --i) {
        TEST_ASSERT_EQUAL(i, *(int *)generic_vector_back(vector, NULL));
        generic_vector_pop(vector);
    }

    TEST_ASSERT_EQUAL(512, vector->capacity);
}

test(order, str) {
    for (int i = 0; i <= 10; ++i)
        generic_vector_push(vector, &str_list[i]);

    for (int i = 10; i >= 0; --i) {
        TEST_ASSERT_EQUAL_STRING(str_list[i],
                                 *(char **)generic_vector_back(vector, NULL));
        generic_vector_pop(vector);
    }
    TEST_ASSERT_EQUAL(16, vector->capacity);
}

test(reserve, int) {
    TEST_ASSERT_EQUAL(-EFAULT, generic_vector_reserve(NULL, 10));
    TEST_ASSERT_EQUAL(-EINVAL, generic_vector_reserve(vector, -10));
    TEST_ASSERT_EQUAL(0, generic_vector_reserve(vector, 10));

    for (int i = 0; i < 16; ++i)
        generic_vector_push(vector, &i);

    TEST_ASSERT_EQUAL(20, vector->capacity);
    TEST_ASSERT_EQUAL(16, vector->size);
    TEST_ASSERT_EQUAL(15, *(int *)generic_vector_back(vector, NULL));

    generic_vector_reserve(vector, 2);
    TEST_ASSERT_EQUAL(20, vector->capacity);
}

test(reserve, str) {
    TEST_ASSERT_EQUAL(0, generic_vector_reserve(vector, 10));
    for (int i = 0; i < 16; ++i)
        generic_vector_push(vector, &str_list[i]);

    TEST_ASSERT_EQUAL(20, vector->capacity);
    TEST_ASSERT_EQUAL(16, vector->size);
    TEST_ASSERT_EQUAL(str_list[15],
                      *(char **)generic_vector_back(vector, NULL));
    generic_vector_reserve(vector, 2);
    TEST_ASSERT_EQUAL(20, vector->capacity);
}

test(get, int) {
    for (int i = 0; i <= 30; ++i) {
        int n = (i << 1) + 1;
        generic_vector_push(vector, &n);
    }
    for (int i = 0; i <= 30; ++i)
        TEST_ASSERT_EQUAL((i << 1) + 1,
                          *(int *)generic_vector_get(vector, i, NULL));

    TEST_ASSERT_EQUAL(32, vector->capacity);
}

test(get, str) {
    for (int i = 0; i < 16; ++i)
        generic_vector_push(vector, &str_list[i]);

    for (int i = 0; i < 16; ++i)
        TEST_ASSERT_EQUAL_STRING(str_list[i],
                                 *(char **)generic_vector_get(vector, i, NULL));
}

int main(void) {
    UNITY_BEGIN();
    size = sizeof(int);
    RUN_TEST(fname(push, int));
    RUN_TEST(fname(back, int));
    RUN_TEST(fname(pop, int));
    RUN_TEST(fname(order, int));
    RUN_TEST(fname(reserve, int));
    RUN_TEST(fname(get, int));

    size = sizeof(char **);
    const char fmt[] = "%d %04d";
    for (int i = 0; i < 16; ++i) {
        int len = snprintf(NULL, 0, fmt, i, i * 10) + 1;
        str_list[i] = __builtin_malloc(len);
        sprintf(str_list[i], fmt, i, i * 10);
    }

    RUN_TEST(fname(push, str));
    RUN_TEST(fname(back, str));
    RUN_TEST(fname(pop, str));
    RUN_TEST(fname(order, str));
    RUN_TEST(fname(reserve, str));
    RUN_TEST(fname(get, str));

    for (int i = 0; i < 16; ++i)
        __builtin_free(str_list[i]);

    UNITY_END();

    return 0;
}
