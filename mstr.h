#ifndef MSTR_H
#define MSTR_H

#include <stddef.h>

/* mstr */

#define MSTR_INIT_CAP 8
#define MSTR_EXPAN_RATIO 2

struct mstr
{
  size_t cap;
  size_t len;
  char *data;
};

typedef struct mstr mstr;

#define mstr_new()                                                            \
  (mstr) { .cap = 0, .len = 0, .data = NULL }

extern void mstr_init (mstr *str) __attribute__ ((nonnull (1)));

extern void mstr_free (mstr *str) __attribute__ ((nonnull (1)));

extern mstr *mstr_reserve (mstr *dest, size_t cap)
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

extern mstr *mstr_cat_bytes (mstr *dest, const char *src, size_t len)
    __attribute__ ((nonnull (1, 2)));

extern mstr *mstr_assign_char (mstr *dest, char src)
    __attribute__ ((nonnull (1)));

extern mstr *mstr_assign_cstr (mstr *dest, const char *src)
    __attribute__ ((nonnull (1, 2)));

extern mstr *mstr_assign_mstr (mstr *dest, const mstr *src)
    __attribute__ ((nonnull (1, 2)));

extern mstr *mstr_assign_bytes (mstr *dest, const char *src, size_t slen)
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

/* mstr_view */

struct mstr_view
{
  size_t len;
  const char *data;
};

typedef struct mstr_view mstr_view;

extern mstr_view *mstr_view_init (mstr_view *view);

static inline mstr_view mstr_view_new (void);
static inline mstr_view mstr_view_new_cstr (const char *src);
static inline mstr_view mstr_view_new_byte (const char *src, size_t slen);

static inline size_t mstr_view_len (const mstr_view *view);
static inline const char *mstr_view_data (const mstr_view *view);

extern mstr_view *mstr_view_bind_cstr (mstr_view *view, const char *src);
extern mstr_view *mstr_view_bind_mstr (mstr_view *view, const mstr *src);
extern mstr_view *mstr_view_bind_byte (mstr_view *view, const char *src,
                                       size_t slen);

extern mstr mstr_view_to_mstr (const mstr_view *view)
    __attribute__ ((warn_unused_result));

#endif
