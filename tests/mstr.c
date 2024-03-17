#include "../mstr.h"
#include <assert.h>
#include <stdbool.h>

static void test_init (void);
static void test_free (void);
static void test_reserve (void);

static void test_cat_char (void);
static void test_cat_cstr (void);
static void test_cat_byte (void);

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

  test_assign_char ();
  test_assign_cstr ();
  test_assign_byte ();

  test_remove ();
  test_substr ();
}

static void
test_init (void)
{
  mstr_t mstr;
  mstr_init (&mstr);

  /* heap.cap == 1 equals to sso.flg == true */
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
  assert (mstr_reserve (&mstr, sso_cap / 4));
  assert (mstr_reserve (&mstr, sso_cap));
  assert (mstr_reserve (&mstr, sso_cap / 2));

  assert (mstr.sso.flg == true);
  assert (mstr.heap.data == NULL);

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
  mstr_t mstr;
  mstr_init (&mstr);

  /* sso */
  for (int i = 0; i < 22; i++)
    assert (mstr_cat_char (&mstr, 'a' + i));

  assert (mstr.sso.flg == true);
  assert (mstr.sso.len == 22);

  /* heap */
  assert (mstr_cat_char (&mstr, 'a' + 22));

  assert (mstr.sso.flg == false);
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
    assert (mstr_cat_cstr (&mstr, "abcd"));
  assert (mstr_cat_cstr (&mstr, "ab"));

  assert (mstr.sso.flg == true);
  assert (mstr.sso.len == 22);

  /* heap */
  assert (mstr_cat_cstr (&mstr, "cd"));

  assert (mstr.sso.flg == false);
  assert (mstr.heap.len == 24);

  mstr_free (&mstr);
}

static void
test_cat_byte (void)
{
  mstr_t mstr;
  mstr_init (&mstr);

  assert (mstr_cat_byte (&mstr, (mstr_byte_t *)"abc\0d", 6));
  assert (mstr_cat_byte (&mstr, (mstr_byte_t *)"abc\0\0", 6));
  assert (mstr.sso.flg == true);
  assert (mstr.sso.len == 12);

  /* heap after the last loop */
  for (int i = 0; i < 5; i++)
    assert (mstr_cat_byte (&mstr, (mstr_byte_t *)"abcd", 4));

  assert (mstr.sso.flg == false);
  assert (mstr.heap.len == 32);

  mstr_free (&mstr);
}

static void
test_assign_char (void)
{
  mstr_t mstr;
  mstr_init (&mstr);

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
  mstr_t mstr;
  mstr_init (&mstr);

  /* all in sso */
  assert (mstr_assign_cstr (&mstr, "hello world! hello c!"));
  assert (mstr_assign_cstr (&mstr, "hello world!"));
  assert (mstr.sso.flg == true);
  assert (mstr.sso.len == 12);

  /* all in heap */
  assert (mstr_assign_cstr (&mstr, "hello world! hello mstr! hello c!"));
  assert (mstr_assign_cstr (&mstr, "hello world!"));
  assert (mstr.sso.flg == false);
  assert (mstr.heap.len == 12);

  mstr_free (&mstr);
}

static void
test_assign_byte (void)
{
  mstr_t mstr;
  mstr_init (&mstr);

  assert (mstr_assign_byte (&mstr, (mstr_byte_t *)"abcdef\0a", 9));
  assert (mstr.sso.len == 9);
  assert (mstr_assign_byte (&mstr, (mstr_byte_t *)"abcdef\0", 8));
  assert (mstr.sso.len == 8);
  assert (mstr_assign_byte (&mstr, (mstr_byte_t *)"abcdef\0", 6));
  assert (mstr.sso.len == 6);

  /* all in sso */
  assert (mstr_assign_cstr (&mstr, "hello world! hello c!"));
  assert (mstr_assign_cstr (&mstr, "hello world!"));
  assert (mstr.sso.flg == true);
  assert (mstr.sso.len == 12);

  /* all in heap */
  assert (mstr_assign_cstr (&mstr, "hello world! hello mstr! hello c!"));
  assert (mstr_assign_cstr (&mstr, "hello world!"));
  assert (mstr.sso.flg == false);
  assert (mstr.heap.len == 12);

  mstr_free (&mstr);
}

static void
test_remove (void)
{
  mstr_t mstr;
  mstr_init (&mstr);

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
  mstr_t mstr, sub;
  mstr_init (&mstr);
  mstr_init (&sub);

  mstr_assign_cstr (&mstr, "Hello World!");
  assert (!mstr_substr (&sub, &mstr, 12, 1));
  assert (mstr_substr (&sub, &mstr, 6, 5));
  assert (mstr_cmp_cstr (&sub, "World") == 0);
  assert (mstr_substr (&sub, &mstr, 6, 8));
  assert (mstr_cmp_cstr (&sub, "World!") == 0);

  mstr_free (&mstr);
  mstr_free (&sub);
}
