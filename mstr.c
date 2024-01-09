#include "mstr.h"
#include <stdlib.h>
#include <string.h>

#define is_sso(str) (str->sso.flag == 1)
#define sso_max_cap (sizeof (mstr) - sizeof (unsigned char))
#define sso_max_len (sso_max_cap - 1)

void
mstr_init (mstr *str)
{
  str->heap.cap = 0;
  str->heap.len = 0;
  str->heap.data = NULL;
  str->sso.flag = 1;
  return;
}

void
mstr_free (mstr *str)
{
  if (!is_sso (str))
    free (str->heap.data);
  mstr_init (str);
  return;
}

mstr *
mstr_reserve (mstr *dest, size_t ncap)
{
  char flag = is_sso (dest);
  if (flag && ncap <= sso_max_cap)
    return dest;

  size_t cap = MSTR_INIT_CAP;
  if (!flag)
    {
      if (ncap <= dest->heap.cap)
        return dest;

      if (dest->heap.cap > cap)
        cap = dest->heap.cap;
    }

  /* compute the capacity to allocate */
  while (cap < ncap)
    cap *= MSTR_EXPAN_RATIO;

  char *data = flag ? malloc (cap * sizeof (char))
                    : realloc (dest->heap.data, cap * sizeof (char));
  if (data == NULL)
    /* malloc failed */
    return NULL;

  if (flag)
    {
      /* copy sso mstr into heap */
      if (memcpy (data, dest->sso.data, dest->sso.len + 1) != data)
        { /* copy failed */
          free (data);
          return NULL;
        }
      /* save the length of sso mstr */
      dest->heap.len = dest->sso.len;
    }

  dest->heap.data = data;
  dest->heap.cap = cap;
  dest->sso.flag = 0;
  return dest;
}

char *
mstr_release (mstr *str)
{
  if (is_sso (str))
    /* sso mstr can not be released */
    return NULL;

  char *data = str->heap.data;
  mstr_init (str);
  return data;
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

  char flag = is_sso (dest);
  size_t len = flag ? dest->sso.len : dest->heap.len;

  if (mstr_reserve (dest, len + 2) != dest)
    /* allocate failed */
    return NULL;

  /* udpate flag */
  flag = is_sso (dest);
  char *data = flag ? dest->sso.data : dest->heap.data;

  data[len] = src;
  data[len + 1] = '\0';

  if (flag)
    dest->sso.len = len + 1;
  else
    dest->heap.len = len + 1;

  return dest;
}

mstr *
mstr_cat_cstr (mstr *dest, const char *src)
{
  size_t slen = strlen (src);
  if (slen == 0)
    /* src is begin with NULL */
    return dest;

  char flag = is_sso (dest);
  size_t len = flag ? dest->sso.len : dest->heap.len;

  if (mstr_reserve (dest, len + slen + 1) != dest)
    /* allocate failed */
    return NULL;

  /* update flag */
  flag = is_sso (dest);
  char *pos = (flag ? dest->sso.data : dest->heap.data) + len;
  if (memmove (pos, src, slen + 1) != pos)
    /* copy failed */
    return NULL;

  if (flag)
    dest->sso.len = len + slen;
  else
    dest->heap.len = len + slen;

  return dest;
}

mstr *
mstr_cat_mstr (mstr *dest, const mstr *src)
{
  if (is_sso (src))
    return mstr_cat_chars (dest, src->sso.data, src->sso.len);
  return mstr_cat_chars (dest, src->heap.data, src->heap.len);
}

mstr *
mstr_cat_chars (mstr *dest, const char *src, size_t slen)
{
  const char *find = memchr (src, '\0', slen);
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

  char flag = is_sso (dest);
  size_t len = flag ? dest->sso.len : dest->heap.len;

  if (mstr_reserve (dest, len + slen + 1) != dest)
    /* allocate failed */
    return NULL;

  /* update flag */
  flag = is_sso (dest);
  char *pos = (flag ? dest->sso.data : dest->heap.data) + len;
  if (memmove (pos, src, slen) != pos)
    /* copy failed */
    return NULL;
  pos[slen] = '\0';

  if (flag)
    dest->sso.len = len + slen;
  else
    dest->heap.len = len + slen;

  return dest;
}

mstr *
mstr_assign_char (mstr *dest, char src)
{
  char flag = is_sso (dest);

  if (mstr_reserve (dest, 2) != dest)
    /* allocate failed */
    return NULL;

  /* update flag */
  flag = is_sso (dest);
  char *data = flag ? dest->sso.data : dest->heap.data;
  data[0] = src;
  data[1] = '\0';

  if (flag)
    dest->sso.len = src == '\0' ? 0 : 1;
  else
    dest->heap.len = src == '\0' ? 0 : 1;

  return dest;
}

mstr *
mstr_assign_cstr (mstr *dest, const char *src)
{
  char flag = is_sso (dest);
  size_t slen = strlen (src);

  if (mstr_reserve (dest, slen + 1) != dest)
    /* allocate failed */
    return NULL;

  /* update flag */
  flag = is_sso (dest);
  char *data = flag ? dest->sso.data : dest->heap.data;
  if (memmove (data, src, slen + 1) != data)
    /* copy failed */
    return NULL;

  if (flag)
    dest->sso.len = slen;
  else
    dest->heap.len = slen;

  return dest;
}

mstr *
mstr_assign_mstr (mstr *dest, const mstr *src)
{
  if (is_sso (src))
    return mstr_assign_chars (dest, src->sso.data, src->sso.len);
  return mstr_assign_chars (dest, src->heap.data, src->heap.len);
}

mstr *
mstr_assign_chars (mstr *dest, const char *src, size_t slen)
{
  const char *find = memchr (src, '\0', slen);
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

  char flag = is_sso (dest);
  if (mstr_reserve (dest, slen + 1) != dest)
    /* allocate failed */
    return NULL;

  /* update flag */
  flag = is_sso (dest);
  char *data = flag ? dest->sso.data : dest->heap.data;
  if (memmove (data, src, slen) != data)
    /* copy failed */
    return NULL;
  data[slen] = '\0';

  if (flag)
    dest->sso.len = slen;
  else
    dest->heap.len = slen;

  return dest;
}

mstr *
mstr_remove (mstr *dest, size_t spos, size_t slen)
{
  char flag = is_sso (dest);
  size_t len = flag ? dest->sso.len : dest->heap.len;
  if (spos >= len)
    /* out of range */
    return NULL;

  if (slen > len - spos)
    slen = len - spos;

  char *data = flag ? dest->sso.data : dest->heap.data;
  char *rmst = data + spos;
  char *cpst = rmst + slen;
  size_t cplen = len - spos - slen;
  if (memmove (rmst, cpst, cplen) != rmst)
    /* move failed */
    return NULL;

  if (flag)
    dest->sso.len -= slen;
  else
    dest->heap.len -= slen;

  return dest;
}

mstr *
mstr_sub (mstr *dest, const mstr *src, size_t spos, size_t slen)
{
  char flag = is_sso (src);
  size_t len = flag ? src->sso.len : src->heap.len;
  if (spos >= len)
    /* out of range */
    return NULL;

  if (slen > len - spos)
    slen = len - spos;

  if (flag)
    return mstr_assign_chars (dest, src->sso.data + spos, slen);
  return mstr_assign_chars (dest, src->heap.data, slen);
}

int
mstr_cmp_cstr (const mstr *lhs, const char *rhs)
{
  char flag = is_sso (lhs);
  if (flag)
    return strcmp (lhs->sso.data, rhs);
  return strcmp (lhs->heap.data, rhs);
}

int
mstr_cmp_mstr (const mstr *lhs, const mstr *rhs)
{
  char flag1 = is_sso (lhs);
  char flag2 = is_sso (rhs);
  const char *data1 = flag1 ? lhs->sso.data : lhs->heap.data;
  const char *data2 = flag2 ? rhs->sso.data : rhs->heap.data;
  return strcmp (data1, data2);
}
