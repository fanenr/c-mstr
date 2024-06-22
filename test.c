#include "mstr.h"
#include <assert.h>
#include <stdbool.h>

static void test_init (void);
static void test_free (void);
static void test_reserve (void);

static void test_cat_char (void);
static void test_cat_cstr (void);
static void test_cat_byte (void);

static void test_insert_char (void);
static void test_insert_cstr (void);
static void test_insert_byte (void);

static void test_assign_char (void);
static void test_assign_cstr (void);
static void test_assign_byte (void);

static void test_remove (void);
static void test_substr (void);

int
main (void)
{
  test_init ();
  test_free ();
  test_reserve ();

  test_cat_char ();
  test_cat_cstr ();
  test_cat_byte ();

  test_insert_char ();
  test_insert_cstr ();
  test_insert_byte ();

  test_assign_char ();
  test_assign_cstr ();
  test_assign_byte ();

  test_remove ();
  test_substr ();
}

static void
test_init (void)
{
  mstr_t mstr = MSTR_INIT;

  assert (mstr.heap.cap % 2);
  assert (mstr.heap.len == 0);
  assert (mstr.heap.data == NULL);
}

static void
test_free (void)
{
  mstr_t mstr = MSTR_INIT;

  mstr_free (&mstr);
}

static void
test_reserve (void)
{
  mstr_t mstr = MSTR_INIT;

  /* all in sso */
  const int sso_cap = sizeof (mstr_t) - sizeof (char);
  assert (mstr_reserve (&mstr, sso_cap / 4));
  assert (mstr_reserve (&mstr, sso_cap));
  assert (mstr_reserve (&mstr, sso_cap / 2));

  assert (mstr.heap.data == NULL);
  assert (mstr.sso.flg == MSTR_FLG_SSO);

  /* all in heap */
  assert (mstr_reserve (&mstr, sso_cap * 2));
  assert (mstr_reserve (&mstr, sso_cap / 2));

  assert (mstr.sso.flg == false);
  assert (mstr.heap.data != NULL);

  /* remember to free mstr */
  mstr_free (&mstr);
}

static void
test_cat_char (void)
{
  mstr_t mstr = MSTR_INIT;

  /* sso */
  for (int i = 0; i < 22; i++)
    assert (mstr_cat_char (&mstr, 'a' + i));

  assert (mstr.sso.len == 22);
  assert (mstr.sso.flg == MSTR_FLG_SSO);

  /* heap */
  assert (mstr_cat_char (&mstr, 'a' + 22));

  assert (mstr.heap.len == 23);
  assert (mstr.sso.flg == MSTR_FLG_HEAP);

  mstr_free (&mstr);
}

static void
test_cat_cstr (void)
{
  mstr_t mstr = MSTR_INIT;

  /* sso */
  for (int i = 0; i < 5; i++)
    assert (mstr_cat_cstr (&mstr, "abcd"));
  assert (mstr_cat_cstr (&mstr, "ab"));

  assert (mstr.sso.len == 22);
  assert (mstr.sso.flg == MSTR_FLG_SSO);

  /* heap */
  assert (mstr_cat_cstr (&mstr, "cd"));

  assert (mstr.heap.len == 24);
  assert (mstr.sso.flg == MSTR_FLG_HEAP);

  mstr_free (&mstr);
}

static void
test_cat_byte (void)
{
  mstr_t mstr = MSTR_INIT;

  assert (mstr_cat_byte (&mstr, "abc\0d", 6));
  assert (mstr_cat_byte (&mstr, "abc\0\0", 6));
  assert (mstr.sso.flg == true);
  assert (mstr.sso.len == 12);

  /* heap after the last loop */
  for (int i = 0; i < 5; i++)
    assert (mstr_cat_byte (&mstr, "abcd", 4));

  assert (mstr.heap.len == 32);
  assert (mstr.sso.flg == MSTR_FLG_HEAP);

  mstr_free (&mstr);
}

static void
test_insert_char (void)
{
  mstr_t mstr = MSTR_INIT;

  /* sso */
  for (int i = 0; i < 22; i++)
    assert (mstr_insert_char (&mstr, i, 'a' + i));

  assert (mstr.sso.len == 22);
  assert (mstr.sso.flg == MSTR_FLG_SSO);

  /* heap */
  assert (mstr_insert_char (&mstr, 0, 'a' + 22));

  assert (mstr.heap.len == 23);
  assert (mstr.sso.flg == MSTR_FLG_HEAP);

  mstr_free (&mstr);
}

static void
test_insert_cstr (void)
{
  mstr_t mstr = MSTR_INIT;

  /* sso */
  for (int i = 0; i < 5; i++)
    assert (mstr_insert_cstr (&mstr, i * 4, "abcd"));
  assert (mstr_insert_cstr (&mstr, 0, "ab"));

  assert (mstr.sso.len == 22);
  assert (mstr.sso.flg == MSTR_FLG_SSO);

  /* heap */
  assert (mstr_insert_cstr (&mstr, 2, "cd"));

  assert (mstr.heap.len == 24);
  assert (mstr.sso.flg == MSTR_FLG_HEAP);

  mstr_free (&mstr);
}

static void
test_insert_byte (void)
{
  mstr_t mstr = MSTR_INIT;

  assert (mstr_insert_byte (&mstr, 0, "abc\0d", 6));
  assert (mstr_insert_byte (&mstr, 0, "abc\0\0", 6));
  assert (mstr.sso.flg == true);
  assert (mstr.sso.len == 12);

  /* heap after the last loop */
  for (int i = 0; i < 5; i++)
    assert (mstr_insert_byte (&mstr, 0, "abcd", 4));

  assert (mstr.heap.len == 32);
  assert (mstr.sso.flg == MSTR_FLG_HEAP);

  mstr_free (&mstr);
}

static void
test_assign_char (void)
{
  mstr_t mstr = MSTR_INIT;

  /* all in sso */
  assert (mstr_assign_char (&mstr, 'a'));
  assert (mstr_assign_char (&mstr, 'b'));
  assert (mstr.sso.flg == true);
  assert (mstr.sso.len == 1);

  mstr_reserve (&mstr, 24);

  /* all in heap */
  assert (mstr_assign_char (&mstr, 'a'));
  assert (mstr_assign_char (&mstr, 'b'));
  assert (mstr.sso.flg == false);
  assert (mstr.heap.len == 1);

  mstr_free (&mstr);
}

static void
test_assign_cstr (void)
{
  mstr_t mstr = MSTR_INIT;

  /* all in sso */
  assert (mstr_assign_cstr (&mstr, "hello world! hello c!"));
  assert (mstr_assign_cstr (&mstr, "hello world!"));
  assert (mstr.sso.flg == MSTR_FLG_SSO);
  assert (mstr.sso.len == 12);

  /* all in heap */
  assert (mstr_assign_cstr (&mstr, "hello world! hello mstr! hello c!"));
  assert (mstr_assign_cstr (&mstr, "hello world!"));
  assert (mstr.sso.flg == MSTR_FLG_HEAP);
  assert (mstr.heap.len == 12);

  mstr_free (&mstr);
}

static void
test_assign_byte (void)
{
  mstr_t mstr = MSTR_INIT;

  assert (mstr_assign_byte (&mstr, "abcdef\0a", 9));
  assert (mstr.sso.len == 9);
  assert (mstr_assign_byte (&mstr, "abcdef\0", 8));
  assert (mstr.sso.len == 8);
  assert (mstr_assign_byte (&mstr, "abcdef\0", 6));
  assert (mstr.sso.len == 6);

  /* all in sso */
  assert (mstr_assign_cstr (&mstr, "hello world! hello c!"));
  assert (mstr_assign_cstr (&mstr, "hello world!"));
  assert (mstr.sso.flg == MSTR_FLG_SSO);
  assert (mstr.sso.len == 12);

  /* all in heap */
  assert (mstr_assign_cstr (&mstr, "hello world! hello mstr! hello c!"));
  assert (mstr_assign_cstr (&mstr, "hello world!"));
  assert (mstr.sso.flg == MSTR_FLG_HEAP);
  assert (mstr.heap.len == 12);

  mstr_free (&mstr);
}

static void
test_remove (void)
{
  mstr_t mstr = MSTR_INIT;

  mstr_assign_cstr (&mstr, "Hello World!");
  assert (mstr_remove (&mstr, 12, 0));
  assert (mstr_remove (&mstr, 5, 6));
  assert (mstr_cmp_cstr (&mstr, "Hello!") == 0);
  assert (mstr_remove (&mstr, 3, 5));
  assert (mstr_cmp_cstr (&mstr, "Hel") == 0);

  mstr_free (&mstr);
}

static void
test_substr (void)
{
  mstr_t mstr = MSTR_INIT;
  mstr_t sub = MSTR_INIT;

  mstr_assign_cstr (&mstr, "Hello World!");
  assert (!mstr_substr (&sub, &mstr, 12, 1));
  assert (mstr_substr (&sub, &mstr, 6, 5));
  assert (mstr_cmp_cstr (&sub, "World") == 0);
  assert (mstr_substr (&sub, &mstr, 6, 8));
  assert (mstr_cmp_cstr (&sub, "World!") == 0);

  mstr_free (&mstr);
  mstr_free (&sub);
}
