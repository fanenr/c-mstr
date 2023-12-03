#include "../mstr.h"
#include <assert.h>

static void test_init(void);
static void test_reserve(void);
static void test_cat_char(void);
static void test_cat_cstr(void);
static void test_assign_cstr(void);
static void test_free(void);

int
main(void)
{
    test_init();
    test_reserve();
    test_cat_char();
    test_cat_cstr();
    test_assign_cstr();
    test_free();
    return 0;
}

static void
test_init(void)
{
    mstr str1 = mstr_new();

    assert(str1.len == 0);
    assert(str1.cap == 0);
    assert(str1.data == NULL);

    return;
}

static void
test_reserve(void)
{
    mstr str1 = mstr_new();
    mstr_reserve(&str1, MSTR_INIT_CAP);

    assert(mstr_cap(&str1) == MSTR_INIT_CAP);
    assert(str1.cap == MSTR_INIT_CAP);
    assert(str1.data != NULL);
    assert(str1.len == 0);

    mstr_free(&str1);
    return;
}

static void
test_cat_char(void)
{
    mstr str1 = mstr_new();
    for (char ch = 'a'; ch <= 'z'; ch++)
        assert(mstr_cat_char(&str1, ch) == &str1);

    assert(str1.len == 26);
    assert(str1.cap == 32);

    mstr str2;
    mstr_init(&str2);
    mstr_reserve(&str2, 30);

    for (char ch = 'a'; ch <= 'z'; ch++)
        assert(mstr_cat_char(&str2, ch) == &str2);

    assert(str2.len == 26);
    assert(str2.cap == 30);

    mstr_free(&str1);
    mstr_free(&str2);
    return;
}

static void
test_cat_cstr(void)
{
    mstr str1 = mstr_new();
    mstr_cat_cstr(&str1, "abc");
    mstr_cat_cstr(&str1, "def");

    assert(str1.len == 6);
    assert(str1.cap == 7);

    mstr_cat_cstr(&str1, "");

    assert(str1.len == 6);
    assert(str1.cap == 7);

    mstr_cat_cstr(&str1, NULL);

    assert(str1.len == 6);
    assert(str1.cap == 7);

    mstr_free(&str1);
    return;
}

static void
test_assign_cstr(void)
{
    mstr str1 = mstr_new();
    mstr_assign_cstr(&str1, "abcdef");

    assert(str1.len == 6);
    assert(str1.cap == MSTR_INIT_CAP);

    /* assign NULL to str1 */
    mstr_assign_cstr(&str1, NULL);

    assert(str1.len == 0);
    assert(str1.cap == 0);
    assert(str1.data == NULL);

    /* assign "" to str1 */
    mstr_assign_cstr(&str1, "abcdef");
    mstr_assign_cstr(&str1, "");

    assert(str1.len == 0);
    assert(str1.cap == 0);
    assert(str1.data == NULL);

    /* mstr_free(&str1); */
    return;
}

static void
test_free(void)
{
    mstr str1 = mstr_new();
    mstr_free(&str1);
    return;
}
