#include "mstr.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define MSTR_SSO_MAXCAP (sizeof (mstr_t) - sizeof (char))

#define is_sso(str) ((str)->sso.flag)
#define get_len(str) (is_sso (str) ? (str)->sso.len : (str)->heap.len)
#define get_cap(str) (is_sso (str) ? MSTR_SSO_MAXCAP : (str)->heap.cap)
#define get_data(str) (is_sso (str) ? (str)->sso.data : (str)->heap.data)

thread_local mstr_errno_t mstr_errno = MSTR_ERR_NONE;

void
mstr_init (mstr_t *str)
{
  *str = (mstr_t)(mstr_sso_t){ .flag = true };
}

void
mstr_free (mstr_t *str)
{
  if (!is_sso (str))
    free (str->heap.data);
  mstr_init (str);
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

int
mstr_at (const mstr_t *str, size_t pos)
{
  if (pos >= get_len (str))
    return -1;
  return get_data (str)[pos];
}

bool
mstr_start_with_char (const mstr_t *str, char src)
{
  return mstr_start_with_byte (str, (const unsigned char *)&src, 1);
}

bool
mstr_start_with_cstr (const mstr_t *str, const char *src)
{
  return mstr_start_with_byte (str, (const unsigned char *)src, strlen (src));
}

bool
mstr_start_with_mstr (const mstr_t *str, const mstr_t *src)
{
  return mstr_start_with_byte (str, (const unsigned char *)get_data (src),
                               get_len (src));
}

bool
mstr_start_with_byte (const mstr_t *str, const unsigned char *src, size_t slen)
{
  if (slen > get_len (str))
    return false;

  return memcmp (get_data (str), src, slen);
}

bool
mstr_end_with_char (const mstr_t *str, char src)
{
  return mstr_end_with_byte (str, (const unsigned char *)&src, 1);
}

bool
mstr_end_with_cstr (const mstr_t *str, const char *src)
{
  return mstr_end_with_byte (str, (const unsigned char *)src, strlen (src));
}

bool
mstr_end_with_mstr (const mstr_t *str, const mstr_t *src)
{
  return mstr_end_with_byte (str, (const unsigned char *)get_data (src),
                             get_len (src));
}

bool
mstr_end_with_byte (const mstr_t *str, const unsigned char *src, size_t slen)
{
  if (slen > get_len (str))
    return false;

  const char *data = get_data (str) + get_len (str) - slen;
  return memcmp (data, src, slen);
}

int
mstr_cmp_cstr (const mstr_t *str, const char *src)
{
  return mstr_cmp_byte (str, (const unsigned char *)src, strlen (src));
}

int
mstr_cmp_mstr (const mstr_t *str, const mstr_t *src)
{
  return mstr_cmp_byte (str, (const unsigned char *)get_data (src),
                        get_len (src));
}

int
mstr_cmp_byte (const mstr_t *str, const unsigned char *src, size_t slen)
{
  size_t len = get_len (str);
  const char *data = get_data (str);
  int cmp = memcmp (data, src, slen > len ? len : slen);

  if (cmp != 0 || slen == len)
    return cmp;
  return len > slen ? 1 : -1;
}

mstr_t *
mstr_reserve (mstr_t *dest, size_t cap)
{
  bool flag = is_sso (dest);
  if (flag && cap <= MSTR_SSO_MAXCAP)
    return dest;
  if (!flag && cap <= dest->heap.cap)
    return dest;

  size_t ncap = MSTR_INIT_CAP;
  if (!flag && dest->heap.cap > ncap)
    ncap = dest->heap.cap;

  /* compute the capacity to allocate */
  while (ncap < cap)
    ncap *= MSTR_EXPAN_RATIO;

  char *data = flag ? malloc (ncap * sizeof (char))
                    : realloc (dest->heap.data, ncap * sizeof (char));
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
  dest->heap.cap = ncap;
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
  return mstr_cat_byte (dest, (const unsigned char *)get_data (src),
                        get_len (src));
}

mstr_t *
mstr_cat_byte (mstr_t *dest, const unsigned char *src, size_t slen)
{
  const unsigned char *find = memchr (src, '\0', slen);
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
  return mstr_assign_byte (dest, (const unsigned char *)get_data (src),
                           get_len (src));
}

mstr_t *
mstr_assign_byte (mstr_t *dest, const unsigned char *src, size_t slen)
{
  const unsigned char *find = memchr (src, '\0', slen);
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
  size_t cplen = len - spos - slen + 1;
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
mstr_substr (mstr_t *dest, const mstr_t *src, size_t spos, size_t slen)
{
  bool flag = is_sso (src);
  size_t len = flag ? src->sso.len : src->heap.len;
  if (spos >= len)
    /* out of range */
    return NULL;

  if (slen > len - spos)
    slen = len - spos;

  const char *pos = (flag ? src->sso.data : src->heap.data) + spos;
  return mstr_assign_byte (dest, (const unsigned char *)pos, slen);
}
