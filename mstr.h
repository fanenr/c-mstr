#ifndef MSTR_H
#define MSTR_H

#include <stdbool.h>
#include <stddef.h>
#include <threads.h>

typedef struct mstr_heap_t
{
  size_t cap;
  size_t len;
  char *data;
} mstr_heap_t;

typedef struct mstr_sso_t
{
  unsigned char flag : 1;
  unsigned char len : 7;
  char data[sizeof (mstr_heap_t) - 1];
} mstr_sso_t;

typedef union mstr_t
{
  mstr_sso_t sso;
  mstr_heap_t heap;
} mstr_t;

typedef enum
{
  MSTR_ERR_NONE,
} mstr_errno_t;

extern thread_local mstr_errno_t mstr_errno;

#define MSTR_EXPAN_RATIO 2
#define MSTR_INIT_CAP (sizeof (mstr_t))

extern void mstr_init (mstr_t *str) __attribute__ ((nonnull (1)));

extern void mstr_free (mstr_t *str) __attribute__ ((nonnull (1)));

extern size_t mstr_cap (const mstr_t *str) __attribute__ ((nonnull (1)));

extern size_t mstr_len (const mstr_t *str) __attribute__ ((nonnull (1)));

extern const char *mstr_data (const mstr_t *str) __attribute__ ((nonnull (1)));

extern int mstr_at (const mstr_t *str, size_t pos)
    __attribute__ ((nonnull (1)));

extern char *mstr_release (mstr_t *str) __attribute__ ((nonnull (1)));

extern mstr_t *mstr_reserve (mstr_t *dest, size_t cap)
    __attribute__ ((nonnull (1)));

extern mstr_t *mstr_remove (mstr_t *dest, size_t spos, size_t slen)
    __attribute__ ((nonnull (1)));

extern void mstr_swap (mstr_t *dest, mstr_t *src)
    __attribute__ ((nonnull (1, 2)));

extern mstr_t *mstr_move (mstr_t *dest, mstr_t *src)
    __attribute__ ((nonnull (1, 2)));

extern mstr_t *mstr_substr (mstr_t *dest, const mstr_t *src, size_t spos,
                            size_t slen) __attribute__ ((nonnull (1, 2)));

#define mstr_start_with(dest, src, ...)                                       \
  _Generic ((src),                                                            \
      int: mstr_start_with_char,                                              \
      char *: mstr_start_with_cstr,                                           \
      mstr_t *: mstr_start_with_mstr,                                         \
      unsigned char *: mstr_start_with_byte) (dest, src, ##__VA_ARGS__)

extern bool mstr_start_with_char (const mstr_t *str, char src)
    __attribute__ ((nonnull (1)));

extern bool mstr_start_with_cstr (const mstr_t *str, const char *src)
    __attribute__ ((nonnull (1, 2)));

extern bool mstr_start_with_mstr (const mstr_t *str, const mstr_t *src)
    __attribute__ ((nonnull (1)));

extern bool mstr_start_with_byte (const mstr_t *str, const unsigned char *src,
                                  size_t slen) __attribute__ ((nonnull (1)));

#define mstr_end_with(dest, src, ...)                                         \
  _Generic ((src),                                                            \
      int: mstr_end_with_char,                                                \
      char *: mstr_end_with_cstr,                                             \
      mstr_t *: mstr_end_with_mstr,                                           \
      unsigned char *: mstr_end_with_byte) (dest, src, ##__VA_ARGS__)

extern bool mstr_end_with_char (const mstr_t *str, char src)
    __attribute__ ((nonnull (1)));

extern bool mstr_end_with_cstr (const mstr_t *str, const char *src)
    __attribute__ ((nonnull (1, 2)));

extern bool mstr_end_with_mstr (const mstr_t *str, const mstr_t *src)
    __attribute__ ((nonnull (1)));

extern bool mstr_end_with_byte (const mstr_t *str, const unsigned char *src,
                                size_t slen) __attribute__ ((nonnull (1)));

#define mstr_cmp(dest, src, ...)                                              \
  _Generic ((src),                                                            \
      char *: mstr_cmp_cstr,                                                  \
      mstr_t *: mstr_cmp_mstr,                                                \
      unsigned char *: mstr_cmp_byte) (dest, src, ##__VA_ARGS__)

extern int mstr_cmp_cstr (const mstr_t *str, const char *src)
    __attribute__ ((nonnull (1, 2)));

extern int mstr_cmp_mstr (const mstr_t *str, const mstr_t *src)
    __attribute__ ((nonnull (1, 2)));

extern int mstr_cmp_byte (const mstr_t *str, const unsigned char *src,
                          size_t slen) __attribute__ ((nonnull (1, 2)));

#define mstr_cat(dest, src, ...)                                              \
  _Generic ((src),                                                            \
      int: mstr_cat_char,                                                     \
      char *: mstr_cat_cstr,                                                  \
      mstr_t *: mstr_cat_mstr,                                                \
      unsigned char *: mstr_cat_byte) (dest, src, ##__VA_ARGS__)

extern mstr_t *mstr_cat_char (mstr_t *dest, char src)
    __attribute__ ((nonnull (1)));

extern mstr_t *mstr_cat_cstr (mstr_t *dest, const char *src)
    __attribute__ ((nonnull (1, 2)));

extern mstr_t *mstr_cat_mstr (mstr_t *dest, const mstr_t *src)
    __attribute__ ((nonnull (1, 2)));

extern mstr_t *mstr_cat_byte (mstr_t *dest, const unsigned char *src,
                              size_t slen) __attribute__ ((nonnull (1, 2)));

#define mstr_assign(dest, src, ...)                                           \
  _Generic ((src),                                                            \
      int: mstr_assign_char,                                                  \
      char *: mstr_assign_cstr,                                               \
      mstr_t *: mstr_assign_mstr,                                             \
      unsigned char *: mstr_assign_byte) (dest, src, ##__VA_ARGS__)

extern mstr_t *mstr_assign_char (mstr_t *dest, char src)
    __attribute__ ((nonnull (1)));

extern mstr_t *mstr_assign_cstr (mstr_t *dest, const char *src)
    __attribute__ ((nonnull (1, 2)));

extern mstr_t *mstr_assign_mstr (mstr_t *dest, const mstr_t *src)
    __attribute__ ((nonnull (1, 2)));

extern mstr_t *mstr_assign_byte (mstr_t *dest, const unsigned char *src,
                                 size_t slen) __attribute__ ((nonnull (1, 2)));

#endif
