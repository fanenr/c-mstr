#ifndef MSTR_H
#define MSTR_H

#include <stddef.h>

/* mstr */

struct mstr;

typedef struct mstr mstr;

extern void mstr_init (mstr *str) __attribute__ ((nonnull (1)));

extern void mstr_free (mstr *str) __attribute__ ((nonnull (1)));

extern mstr *mstr_reserve (mstr *dest, size_t ncap)
    __attribute__ ((nonnull (1)));

extern char *mstr_release (mstr *str)
    __attribute__ ((nonnull (1), warn_unused_result));

extern mstr *mstr_move_cstr (mstr *dest, char *src)
    __attribute__ ((nonnull (1, 2)));

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

extern mstr *mstr_remove (mstr *dest, size_t spos, size_t len)
    __attribute__ ((nonnull (1)));

extern mstr mstr_substr (const mstr *dest, size_t spos, size_t len)
    __attribute__ ((nonnull (1), warn_unused_result));

#define MSTR_CMP_EQ 0
#define MSTR_CMP_GT 1
#define MSTR_CMP_LT -1

extern int mstr_cmp_cstr (const mstr *lhs, const char *rhs)
    __attribute__ ((nonnull (1, 2)));

extern int mstr_cmp_mstr (const mstr *lhs, const mstr *rhs)
    __attribute__ ((nonnull (1, 2)));

#endif
