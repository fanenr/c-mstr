#ifndef MSTR_H
#define MSTR_H

#include <stddef.h>
#include <threads.h>

struct mstr_heap_t
{
  size_t cap;
  size_t len;
  char *data;
};

struct mstr_sso_t
{
  unsigned char flag : 1;
  unsigned char len : 8 * sizeof (unsigned char) - 1;
  char data[sizeof (struct mstr_heap_t) - sizeof (unsigned char)];
};

union mstr_t
{
  struct mstr_sso_t sso;
  struct mstr_heap_t heap;
};

typedef union mstr_t mstr_t;

typedef enum
{
  MSTR_ERR_NONE,
} mstr_errno_t;

extern thread_local mstr_errno_t mstr_errno;

#define MSTR_INIT_CAP 8
#define MSTR_EXPAN_RATIO 2

extern void mstr_init (mstr_t *str) __attribute__ ((nonnull (1)));

extern void mstr_free (mstr_t *str) __attribute__ ((nonnull (1)));

extern size_t mstr_cap (const mstr_t *str) __attribute__ ((nonnull (1)));

extern size_t mstr_len (const mstr_t *str) __attribute__ ((nonnull (1)));

extern const char *mstr_data (const mstr_t *str) __attribute__ ((nonnull (1)));

extern mstr_t *mstr_reserve (mstr_t *dest, size_t ncap)
    __attribute__ ((nonnull (1)));

extern char *mstr_release (mstr_t *str)
    __attribute__ ((nonnull (1), warn_unused_result));

extern mstr_t *mstr_move_mstr (mstr_t *dest, mstr_t *src)
    __attribute__ ((nonnull (1, 2)));

extern void mstr_swap (mstr_t *dest, mstr_t *src)
    __attribute__ ((nonnull (1, 2)));

extern mstr_t *mstr_cat_char (mstr_t *dest, char src)
    __attribute__ ((nonnull (1)));

extern mstr_t *mstr_cat_cstr (mstr_t *dest, const char *src)
    __attribute__ ((nonnull (1, 2)));

extern mstr_t *mstr_cat_mstr (mstr_t *dest, const mstr_t *src)
    __attribute__ ((nonnull (1, 2)));

extern mstr_t *mstr_cat_chars (mstr_t *dest, const char *src, size_t slen)
    __attribute__ ((nonnull (1, 2)));

extern mstr_t *mstr_assign_char (mstr_t *dest, char src)
    __attribute__ ((nonnull (1)));

extern mstr_t *mstr_assign_cstr (mstr_t *dest, const char *src)
    __attribute__ ((nonnull (1, 2)));

extern mstr_t *mstr_assign_mstr (mstr_t *dest, const mstr_t *src)
    __attribute__ ((nonnull (1, 2)));

extern mstr_t *mstr_assign_chars (mstr_t *dest, const char *src, size_t slen)
    __attribute__ ((nonnull (1, 2)));

extern mstr_t *mstr_remove (mstr_t *dest, size_t spos, size_t slen)
    __attribute__ ((nonnull (1)));

extern mstr_t *mstr_substr (mstr_t *dest, const mstr_t *src, size_t spos,
                            size_t slen) __attribute__ ((nonnull (1, 2)));

extern int mstr_cmp_cstr (const mstr_t *lhs, const char *rhs)
    __attribute__ ((nonnull (1, 2)));

extern int mstr_cmp_mstr (const mstr_t *lhs, const mstr_t *rhs)
    __attribute__ ((nonnull (1, 2)));

#endif
