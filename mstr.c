#include "mstr.h"
#include <stdlib.h>
#include <string.h>

void
mstr_init (mstr *str)
{
  str->len = str->cap = 0;
  str->data = NULL;
  return;
}

void
mstr_free (mstr *str)
{
  free (str->data);
  mstr_init (str);
  return;
}

mstr *
mstr_reserve (mstr *dest, size_t ncap)
{
  size_t cap = dest->cap;
  if (ncap <= cap)
    return dest;

  if (ncap < MSTR_INIT_CAP)
    /* at least MSTR_INIT_CAP */
    ncap = MSTR_INIT_CAP;

  while (cap < ncap)
    cap = cap * MSTR_EXPAN_RATIO;

  char *data = realloc (dest->data, cap * sizeof (char));
  if (data == NULL)
    /* allocate failed */
    return NULL;

  dest->data = data;
  dest->cap = cap;
  return dest;
}

char *
mstr_release (mstr *str)
{
  char *data = str->data;
  mstr_init (str);
  return data;
}

mstr *
mstr_move_cstr (mstr *dest, char *src)
{
  size_t len = strlen (src);
  if (len == 0)
    /* src is begin with NULL */
    return NULL;

  mstr_free (dest);

  dest->cap = len + 1;
  dest->data = src;
  dest->len = len;
  return dest;
}

mstr *
mstr_move_mstr (mstr *dest, mstr *src)
{
  mstr_free (dest);
  *dest = *src;
  mstr_init (src);
  return dest;
}

void
mstr_swap (mstr *dest, mstr *src)
{
  mstr copy = *dest;
  *dest = *src;
  *src = copy;
  return;
}

mstr *
mstr_cat_char (mstr *dest, char src)
{
  if (src == '\0')
    return dest;

  size_t len = dest->len;
  if (len + 2 > dest->cap)
    /* expand capacity */
    if (mstr_reserve (dest, len + 2) != dest)
      /* allocate failed */
      return NULL;

  dest->data[len] = src;
  dest->data[len + 1] = '\0';

  dest->len++;
  return dest;
}

mstr *
mstr_cat_cstr (mstr *dest, const char *src)
{
  size_t len = dest->len;
  size_t slen = strlen (src);
  if (slen == 0)
    /* src is begin with NULL */
    return dest;

  if (len + slen + 1 > dest->cap)
    /* expand capacity */
    if (mstr_reserve (dest, len + slen + 1) != dest)
      /* allocate failed */
      return NULL;

  char *pos = dest->data + len;
  if (memmove (pos, src, slen) != pos)
    /* copy failed */
    return NULL;

  dest->len += slen;
  dest->data[dest->len] = '\0';
  return dest;
}

mstr *
mstr_cat_mstr (mstr *dest, const mstr *src)
{
  return mstr_cat_chars (dest, src->data, src->len);
}

mstr *
mstr_cat_chars (mstr *dest, const char *src, size_t slen)
{
  const char *find = memchr (dest, '\0', slen);
  if (find != NULL)
    {
      if (find != (src + slen - 1))
        /* not a valid cstr */
        return NULL;
      /* remove NULL at the end */
      slen -= 1;
    }

  size_t len = dest->len;
  if (len + slen + 1 > dest->cap)
    if (mstr_reserve (dest, len + slen + 1) != dest)
      /* allocate failed */
      return NULL;

  char *pos = dest->data + len;
  if (memmove (pos, src, slen) != pos)
    /* copy failed */
    return NULL;
  pos[slen] = '\0';

  dest->len += slen;
  return dest;
}

mstr *
mstr_assign_char (mstr *dest, char src)
{
  if (dest->cap < 2)
    /* expand capacity */
    if (mstr_reserve (dest, 2) != dest)
      /* allocate failed */
      return NULL;

  dest->data[0] = src;
  dest->data[1] = '\0';

  dest->len = src == '\0' ? 0 : 1;
  return dest;
}

mstr *
mstr_assign_cstr (mstr *dest, const char *src)
{
  size_t slen = strlen (src);

  if (slen + 1 > dest->cap)
    /* expand capacity */
    if (mstr_reserve (dest, slen + 1) != dest)
      /* allocate failed */
      return NULL;

  char *data = dest->data;
  if (memmove (data, src, slen) != data)
    /* copy failed */
    return NULL;
  data[slen] = '\0';

  dest->len = slen;
  return dest;
}

mstr *
mstr_assign_mstr (mstr *dest, const mstr *src)
{
  return mstr_assign_chars (dest, src->data, src->len);
}

mstr *
mstr_assign_chars (mstr *dest, const char *src, size_t slen)
{
  const char *find = memchr (dest, '\0', slen);
  if (find != NULL)
    {
      if (find != (src + slen - 1))
        /* not a valid cstr */
        return NULL;
      /* remove NULL at the end */
      slen -= 1;
    }

  if (slen + 1 > dest->cap)
    /* expand capacity */
    if (mstr_reserve (dest, slen + 1) != dest)
      /* allocate failed */
      return NULL;

  char *data = dest->data;
  if (memmove (data, src, slen) != data)
    /* copy failed */
    return NULL;
  data[slen] = '\0';

  dest->len = slen;
  return dest;
}

mstr *
mstr_remove (mstr *dest, size_t spos, size_t slen)
{
  size_t len = dest->len;
  if (spos >= len)
    /* out of range */
    return NULL;

  if (slen > len - spos)
    slen = len - spos;

  char *data = dest->data;
  char *rmst = data + spos;
  char *cpst = rmst + slen;
  size_t cplen = len - spos - slen;
  if (memmove (rmst, cpst, cplen) != rmst)
    /* move failed */
    return NULL;

  dest->len -= slen;
  return dest;
}

mstr *
mstr_sub (mstr *dest, const mstr *src, size_t spos, size_t slen)
{
  size_t len = src->len;
  if (spos >= len)
    /* out of range */
    return NULL;

  if (slen > len - spos)
    slen = len - spos;

  return mstr_assign_chars (dest, src->data + spos, slen);
}

int
mstr_cmp_cstr (const mstr *lhs, const char *rhs)
{
  return strcmp (lhs->data, rhs);
}

int
mstr_cmp_mstr (const mstr *lhs, const mstr *rhs)
{
  return strcmp (lhs->data, rhs->data);
}

/*
mstr_view *
mstr_view_init (mstr_view *view)
{
  if (view == NULL)
    return NULL;

  view->data = NULL;
  view->len = 0;
  return view;
}

mstr_view *
mstr_view_bind_cstr (mstr_view *view, const char *src)
{
  if (view == NULL || src == NULL)
    return NULL;

  view->len = strlen (src);
  view->data = src;
  return view;
}

mstr_view *
mstr_view_bind_byte (mstr_view *view, const char *src, size_t slen)
{
  if (view == NULL || (src == NULL && slen != 0))
    return NULL;

  view->data = src;
  view->len = slen;
  return view;
}

mstr_view *
mstr_view_bind_mstr (mstr_view *view, const mstr *src)
{
  if (view == NULL || src == NULL || src->len == 0)
    return NULL;

  view->data = src->data;
  view->len = src->len;
  return view;
}

mstr
mstr_view_to_mstr (const mstr_view *view)
{
  if (view == NULL)
    return mstr_new ();
  return mstr_new_byte (view->data, view->len);
}
*/
