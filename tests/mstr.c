#include "../mstr.h"
#include <assert.h>
#include <stdbool.h>

static void test_init (void);
static void test_free (void);
static void test_reserve (void);
static void test_cat_char (void);
static void test_cat_cstr (void);
static void test_cat_chars (void);

int
main (void)
{
  test_init ();
  test_free ();
  test_reserve ();
  test_cat_char ();
  test_cat_cstr ();
  test_cat_chars ();
}

static void
test_init (void)
{
  mstr_t mstr;
  mstr_init (&mstr);

  /* heap.cap == 1 equals to sso.flag == true */
  assert (mstr.heap.cap == 1);
  assert (mstr.heap.len == 0);
  assert (mstr.heap.data == NULL);
}

static void
test_free (void)
{
  mstr_t mstr;
  mstr_init (&mstr);

  mstr_free (&mstr);
}

static void
test_reserve (void)
{
  mstr_t mstr;
  mstr_init (&mstr);

  /* all in sso */
  const int sso_cap = sizeof (mstr_t) - sizeof (char);
  assert (mstr_reserve (&mstr, sso_cap / 4) == &mstr);
  assert (mstr_reserve (&mstr, sso_cap) == &mstr);
  assert (mstr_reserve (&mstr, sso_cap / 2) == &mstr);

  assert (mstr.sso.flag == true);
  assert (mstr.heap.data == NULL);

  /* all in heap */
  assert (mstr_reserve (&mstr, sso_cap * 2) == &mstr);
  assert (mstr_reserve (&mstr, sso_cap / 2) == &mstr);

  assert (mstr.sso.flag == false);
  assert (mstr.heap.data != NULL);

  /* remember to free mstr */
  mstr_free (&mstr);
}

static void
test_cat_char (void)
{
  mstr_t mstr;
  mstr_init (&mstr);

  /* sso */
  for (int i = 0; i < 22; i++)
    assert (mstr_cat_char (&mstr, 'a' + i) == &mstr);

  assert (mstr.sso.flag == true);
  assert (mstr.sso.len == 22);

  /* heap */
  assert (mstr_cat_char (&mstr, 'a' + 22));

  assert (mstr.sso.flag == false);
  assert (mstr.heap.len == 23);

  mstr_free (&mstr);
}

static void
test_cat_cstr (void)
{
  mstr_t mstr;
  mstr_init (&mstr);

  /* sso */
  for (int i = 0; i < 5; i++)
    assert (mstr_cat_cstr (&mstr, "abcd") == &mstr);
  assert (mstr_cat_cstr (&mstr, "ab") == &mstr);

  assert (mstr.sso.flag == true);
  assert (mstr.sso.len == 22);

  /* heap */
  assert (mstr_cat_cstr (&mstr, "cd") == &mstr);

  assert (mstr.sso.flag == false);
  assert (mstr.heap.len == 24);

  mstr_free (&mstr);
}

static void
test_cat_chars (void)
{
  mstr_t mstr;
  mstr_init (&mstr);

  assert (mstr_cat_chars (&mstr, "abc\0d", 6) == NULL);
  assert (mstr_cat_chars (&mstr, "abc\0\0", 6) == &mstr);
  assert (mstr.sso.flag == true);
  assert (mstr.sso.len == 3);

  /* heap after the last loop */
  for (int i = 0; i < 5; i++)
    assert (mstr_cat_chars (&mstr, "abcd", 4));

  assert (mstr.sso.flag == false);
  assert (mstr.heap.len == 23);

  mstr_free (&mstr);
}

