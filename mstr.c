#include "mstr.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define sso_cap (sizeof (mstr_t) - sizeof (unsigned char))

#define is_sso(str) (str->sso.flag == 1)
#define get_cap(str) (is_sso (str) ? sso_cap : str->heap.cap)
#define get_len(str) (is_sso (str) ? str->sso.len : str->heap.len)
#define get_data(str) (is_sso (str) ? str->sso.data : str->heap.data)

thread_local mstr_errno_t mstr_errno = MSTR_ERR_NONE;

void
mstr_init (mstr_t *str)
{
  memset (str, 0, sizeof (mstr_t));
  str->sso.flag = 1;
  return;
}

void
mstr_free (mstr_t *str)
{
  if (!is_sso (str))
    free (str->heap.data);
  mstr_init (str);
  return;
}

size_t
mstr_cap (const mstr_t *str)
{
  return get_cap (str);
}

size_t
mstr_len (const mstr_t *str)
{
  return get_len (str);
}

const char *
mstr_data (const mstr_t *str)
{
  return get_data (str);
}

mstr_t *
mstr_reserve (mstr_t *dest, size_t ncap)
{
  bool flag = is_sso (dest);
  if (flag && ncap <= sso_cap)
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
mstr_release (mstr_t *str)
{
  if (is_sso (str))
    /* sso mstr can not be released */
    return NULL;

  char *data = str->heap.data;
  mstr_init (str);
  return data;
}

mstr_t *
mstr_move_mstr (mstr_t *dest, mstr_t *src)
{
  mstr_t bak = *dest;
  *dest = *src;
  mstr_free (&bak);
  mstr_init (src);
  return dest;
}

void
mstr_swap (mstr_t *dest, mstr_t *src)
{
  mstr_t copy = *dest;
  *dest = *src;
  *src = copy;
  return;
}

mstr_t *
mstr_cat_char (mstr_t *dest, char src)
{
  if (src == '\0')
    return dest;

  bool flag = is_sso (dest);
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

mstr_t *
mstr_cat_cstr (mstr_t *dest, const char *src)
{
  size_t slen = strlen (src);
  if (slen == 0)
    /* src is begin with NULL */
    return dest;

  bool flag = is_sso (dest);
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

mstr_t *
mstr_cat_mstr (mstr_t *dest, const mstr_t *src)
{
  if (is_sso (src))
    return mstr_cat_chars (dest, src->sso.data, src->sso.len);
  return mstr_cat_chars (dest, src->heap.data, src->heap.len);
}

mstr_t *
mstr_cat_chars (mstr_t *dest, const char *src, size_t slen)
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

  bool flag = is_sso (dest);
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

mstr_t *
mstr_assign_char (mstr_t *dest, char src)
{
  bool flag = is_sso (dest);

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

mstr_t *
mstr_assign_cstr (mstr_t *dest, const char *src)
{
  bool flag = is_sso (dest);
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

mstr_t *
mstr_assign_mstr (mstr_t *dest, const mstr_t *src)
{
  if (is_sso (src))
    return mstr_assign_chars (dest, src->sso.data, src->sso.len);
  return mstr_assign_chars (dest, src->heap.data, src->heap.len);
}

mstr_t *
mstr_assign_chars (mstr_t *dest, const char *src, size_t slen)
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

  bool flag = is_sso (dest);
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

mstr_t *
mstr_remove (mstr_t *dest, size_t spos, size_t slen)
{
  bool flag = is_sso (dest);
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
    dest->sso.len = len - slen;
  else
    dest->heap.len = len - slen;

  return dest;
}

mstr_t *
mstr_sub (mstr_t *dest, const mstr_t *src, size_t spos, size_t slen)
{
  bool flag = is_sso (src);
  size_t len = flag ? src->sso.len : src->heap.len;
  if (spos >= len)
    /* out of range */
    return NULL;

  if (slen > len - spos)
    slen = len - spos;

  const char *pos = (flag ? src->sso.data : src->heap.data) + spos;
  return mstr_assign_chars (dest, pos, slen);
}

int
mstr_cmp_cstr (const mstr_t *lhs, const char *rhs)
{
  bool flag = is_sso (lhs);
  size_t len2 = strlen (rhs);
  size_t len1 = flag ? lhs->sso.len : lhs->heap.len;
  const char *data = flag ? lhs->sso.data : lhs->heap.data;
  int cmp = memcmp (data, rhs, len1 > len2 ? len2 : len1);

  if (cmp != 0)
    return cmp;
  return len1 > len2 ? 1 : -1;
}

int
mstr_cmp_mstr (const mstr_t *lhs, const mstr_t *rhs)
{
  bool flag1 = is_sso (lhs);
  bool flag2 = is_sso (rhs);
  size_t len1 = flag1 ? lhs->sso.len : lhs->heap.len;
  size_t len2 = flag2 ? rhs->sso.len : rhs->heap.len;
  const char *data1 = flag1 ? lhs->sso.data : lhs->heap.data;
  const char *data2 = flag2 ? rhs->sso.data : rhs->heap.data;
  int cmp = memcmp (data1, data2, len1 > len2 ? len2 : len1);

  if (cmp != 0)
    return cmp;
  return len1 > len2 ? 1 : -1;
}
