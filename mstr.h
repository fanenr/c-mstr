#ifndef MSTR_H
#define MSTR_H

#include <stddef.h>

struct mstr_heap
{
  size_t cap;
  size_t len;
  char *data;
};

struct mstr_sso
{
  unsigned char flag : 1;
  unsigned char len : 8 * sizeof (unsigned char) - 1;
  char data[sizeof (struct mstr_heap) - sizeof (unsigned char)];
};

union mstr
{
  struct mstr_sso sso;
  struct mstr_heap heap;
};

typedef union mstr mstr;

#define MSTR_INIT_CAP 8
#define MSTR_EXPAN_RATIO 2

extern void mstr_init (mstr *str) __attribute__ ((nonnull (1)));

extern void mstr_free (mstr *str) __attribute__ ((nonnull (1)));

extern size_t mstr_cap (const mstr *str) __attribute__ ((nonnull (1)));

extern size_t mstr_len (const mstr *str) __attribute__ ((nonnull (1)));

extern const char *mstr_data (const mstr *str) __attribute__ ((nonnull (1)));

extern mstr *mstr_reserve (mstr *dest, size_t ncap)
    __attribute__ ((nonnull (1)));

extern char *mstr_release (mstr *str)
    __attribute__ ((nonnull (1), warn_unused_result));

extern mstr *mstr_move_mstr (mstr *dest, mstr *src)
    __attribute__ ((nonnull (1, 2)));

extern void mstr_swap (mstr *dest, mstr *src) __attribute__ ((nonnull (1, 2)));

extern mstr *mstr_cat_char (mstr *dest, char src)
    __attribute__ ((nonnull (1)));

extern mstr *mstr_cat_cstr (mstr *dest, const char *src)
    __attribute__ ((nonnull (1, 2)));

extern mstr *mstr_cat_mstr (mstr *dest, const mstr *src)
    __attribute__ ((nonnull (1, 2)));

extern mstr *mstr_cat_chars (mstr *dest, const char *src, size_t slen)
    __attribute__ ((nonnull (1, 2)));

extern mstr *mstr_assign_char (mstr *dest, char src)
    __attribute__ ((nonnull (1)));

extern mstr *mstr_assign_cstr (mstr *dest, const char *src)
    __attribute__ ((nonnull (1, 2)));

extern mstr *mstr_assign_mstr (mstr *dest, const mstr *src)
    __attribute__ ((nonnull (1, 2)));

extern mstr *mstr_assign_chars (mstr *dest, const char *src, size_t slen)
    __attribute__ ((nonnull (1, 2)));

extern mstr *mstr_remove (mstr *dest, size_t spos, size_t slen)
    __attribute__ ((nonnull (1)));

extern mstr *mstr_substr (mstr *dest, const mstr *src, size_t spos,
                          size_t slen) __attribute__ ((nonnull (1, 2)));

extern int mstr_cmp_cstr (const mstr *lhs, const char *rhs)
    __attribute__ ((nonnull (1, 2)));

extern int mstr_cmp_mstr (const mstr *lhs, const mstr *rhs)
    __attribute__ ((nonnull (1, 2)));

#endif
