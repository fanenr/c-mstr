/* mstr */

#ifndef MSTR_H
#define MSTR_H

#include <stddef.h>

#define MSTR_INIT_CAP 8
#define MSTR_EXPAN_RATIO 2

struct mstr
{
  size_t cap;
  size_t len;
  char *data;
};

typedef struct mstr mstr;

extern mstr *mstr_init (mstr *str);
extern mstr *mstr_free (mstr *str);
extern mstr *mstr_reserve (mstr *dest, size_t cap);

static inline mstr mstr_new (void);
static inline mstr mstr_new_cap (size_t cap);
static inline mstr mstr_new_char (char src);
static inline mstr mstr_new_cstr (const char *src);
static inline mstr mstr_new_byte (const char *src, size_t slen);

static inline size_t mstr_cap (const mstr *str);
static inline size_t mstr_len (const mstr *str);
static inline const char *mstr_data (const mstr *str);

extern char *mstr_release (mstr *str);
extern mstr *mstr_move_cstr (mstr *dest, char *src);
extern mstr *mstr_move_mstr (mstr *dest, mstr *src);
extern mstr *mstr_swap_mstr (mstr *dest, mstr *src);

extern mstr *mstr_cat_char (mstr *dest, char src);
extern mstr *mstr_cat_cstr (mstr *dest, const char *src);
extern mstr *mstr_cat_mstr (mstr *dest, const mstr *src);
extern mstr *mstr_cat_byte (mstr *dest, const char *src, size_t slen);

extern mstr *mstr_assign_char (mstr *dest, char src);
extern mstr *mstr_assign_cstr (mstr *dest, const char *src);
extern mstr *mstr_assign_mstr (mstr *dest, const mstr *src);
extern mstr *mstr_assign_byte (mstr *dest, const char *src, size_t slen);

extern mstr *mstr_remove_from (mstr *dest, size_t spos, size_t len);
extern mstr *mstr_remove_range (mstr *dest, size_t spos, size_t epos);

extern mstr mstr_sub_from (const mstr *dest, size_t spos, size_t len)
    __attribute__ ((warn_unused_result));
extern mstr mstr_sub_range (const mstr *dest, size_t spos, size_t epos)
    __attribute__ ((warn_unused_result));

#define MSTR_CMP_EQ 0
#define MSTR_CMP_GT 1
#define MSTR_CMP_LT -1

extern int mstr_cmp_cstr (const mstr *lhs, const char *rhs);
extern int mstr_cmp_mstr (const mstr *lhs, const mstr *rhs);

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
extern mstr_view *mstr_view_bind_byte (mstr_view *view, const char *src,
                                       size_t slen);

extern mstr mstr_view_to_mstr (const mstr_view *view)
    __attribute__ ((warn_unused_result));

/* inline functions for mstr. */

static inline mstr
mstr_new (void)
{
  mstr str;
  mstr_init (&str);
  return str;
}

static inline mstr
mstr_new_cap (size_t cap)
{
  mstr str;
  mstr_init (&str);
  mstr_reserve (&str, cap);
  return str;
}

static inline mstr
mstr_new_char (char src)
{
  mstr str;
  mstr_init (&str);
  mstr_assign_char (&str, src);
  return str;
}

static inline mstr
mstr_new_cstr (const char *src)
{
  mstr str;
  mstr_init (&str);
  mstr_assign_cstr (&str, src);
  return str;
}

static inline mstr
mstr_new_byte (const char *src, size_t slen)
{
  mstr str;
  mstr_init (&str);
  mstr_assign_byte (&str, src, slen);
  return str;
}

static inline size_t
mstr_cap (const mstr *str)
{
  return str == NULL ? 0 : str->cap;
}

static inline size_t
mstr_len (const mstr *str)
{
  return str == NULL ? 0 : str->len;
}

static inline const char *
mstr_data (const mstr *str)
{
  return str == NULL ? NULL : str->data;
}

/* inline functions for mstr_view. */

static inline mstr_view
mstr_view_new (void)
{
  mstr_view view;
  mstr_view_init (&view);
  return view;
}

static inline mstr_view
mstr_view_new_cstr (const char *src)
{
  mstr_view view;
  mstr_view_init (&view);
  mstr_view_bind_cstr (&view, src);
  return view;
}

static inline mstr_view
mstr_view_new_byte (const char *src, size_t slen)
{
  mstr_view view;
  mstr_view_init (&view);
  mstr_view_bind_byte (&view, src, slen);
  return view;
}

static inline size_t
mstr_view_len (const mstr_view *view)
{
  return view == NULL ? 0 : view->len;
}

static inline const char *
mstr_view_data (const mstr_view *view)
{
  return view == NULL ? NULL : view->data;
}

#endif
