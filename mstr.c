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

  if (cap < MSTR_INIT_CAP)
    /* at least MSTR_INIT_CAP */
    cap = MSTR_INIT_CAP;

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

  /* free dest */
  mstr_free (dest);

  /* make sure capacity is aligned */
  mstr_reserve (dest, len + 1);

  dest->data = src;
  dest->len = len;
  return dest;
}

mstr *
mstr_move_mstr (mstr *dest, mstr *src)
{
  mstr bak = *dest;
  *dest = *src;
  mstr_free (&bak);
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
      size_t suffix = 1;
      size_t remain = slen - (find - src) - 1;
      for (; suffix <= remain; suffix++)
        if (*(++find) != '\0')
          /* not a valid cstr */
          return NULL;
      /* remove NULL at the end */
      slen -= suffix;
    }

  size_t len = dest->len;
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
      size_t suffix = 1;
      size_t remain = slen - (find - src) - 1;
      for (; suffix <= remain; suffix++)
        if (*(++find) != '\0')
          /* not a valid cstr */
          return NULL;
      /* remove NULL at the end */
      slen -= suffix;
    }

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
