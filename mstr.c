#include "mstr.h"
#include <stdlib.h>
#include <string.h>

mstr *
mstr_init(mstr *str)
{
    if (str == NULL)
        return NULL;

    str->data = NULL;
    str->len = str->cap = 0;
    return str;
}

size_t
mstr_cap(const mstr *str)
{
    return str == NULL ? 0 : str->cap;
}

size_t
mstr_len(const mstr *str)
{
    return str == NULL ? 0 : str->len;
}

const char *
mstr_data(const mstr *str)
{
    return str == NULL ? NULL : str->data;
}

char *
mstr_move_cstr(mstr *str)
{
    if (str == NULL)
        return NULL;

    char *cstr = str->data;
    if (mstr_init(str) != str) /* init failed */
        return NULL;

    return cstr;
}

mstr *
mstr_move_mstr(mstr *dest, mstr *src)
{
    if (dest == NULL)
        return NULL;

    if (mstr_free(dest) != dest) /* free failed */
        return NULL;

    if (src == NULL)
        return dest;

    *dest = *src;
    mstr_init(src);
    return dest;
}

mstr *
mstr_reserve(mstr *dest, size_t cap)
{
    if (dest == NULL || cap <= dest->cap)
        return dest;

    char *data = realloc(dest->data, cap);
    if (data == NULL) /* allocate failed */
        return NULL;

    dest->cap = cap;
    dest->data = data;
    return dest;
}

mstr *
mstr_cat_char(mstr *dest, char src)
{
    if (dest == NULL || src == '\0')
        return dest;

    if (dest->cap < dest->len + 2) {
        size_t cap
            = dest->cap > 0 ? dest->cap * MSTR_EXPAN_RATIO : MSTR_INIT_CAP;
        if (mstr_reserve(dest, cap) != dest) /* allocate failed */
            return NULL;
    }

    dest->data[dest->len++] = src;
    dest->data[dest->len] = '\0';
    return dest;
}

mstr *
mstr_cat_cstr(mstr *dest, const char *src)
{
    if (dest == NULL || src == NULL || *src == '\0')
        return dest;

    size_t cap = dest->cap;
    size_t dest_len = dest->len;
    size_t src_len = strlen(src);
    if (cap < dest_len + src_len + 1) {
        cap = cap > 0 ? dest_len + src_len + 1 : src_len + 1;
        if (mstr_reserve(dest, cap) != dest) /* allocate failed */
            return NULL;
    }

    char *pos = dest->data + dest_len;
    if (memmove(pos, src, src_len) != pos) /* copy failed */
        return NULL;

    dest->len += src_len;
    dest->data[dest->len] = '\0';
    return dest;
}

mstr *
mstr_cat_mstr(mstr *dest, const mstr *src)
{
    if (dest == NULL || src == NULL)
        return dest;
    return mstr_cat_cstr(dest, src->data);
}

mstr *
mstr_assign_char(mstr *dest, char src)
{
    if (dest == NULL || src == '\0')
        return mstr_free(dest);

    size_t cap = MSTR_INIT_CAP;
    char *data = malloc(cap);
    if (data == NULL) /* allocate failed */
        return NULL;

    data[0] = src;
    data[1] = '\0';
    if (mstr_free(dest) != dest) { /* free failed */
        free(data);
        return NULL;
    }

    dest->data = data;
    dest->cap = cap;
    dest->len = 2;
    return dest;
}

mstr *
mstr_assign_cstr(mstr *dest, const char *src)
{
    if (dest == NULL || src == NULL || *src == '\0')
        return mstr_free(dest);

    size_t src_len = strlen(src);
    size_t cap = src_len + 1 < MSTR_INIT_CAP ? MSTR_INIT_CAP : src_len + 1;

    char *data = malloc(cap);
    if (data == NULL) /* allocate failed */
        return NULL;

    data[src_len] = '\0';
    if (memmove(data, src, cap) != data) { /* copy failed */
        free(data);
        return NULL;
    }

    if (mstr_free(dest) != dest) /* free failed */
        return NULL;

    dest->len = src_len;
    dest->data = data;
    dest->cap = cap;
    return dest;
}

mstr *
mstr_assign_mstr(mstr *dest, const mstr *src)
{
    if (dest == NULL || src == NULL)
        return mstr_free(dest);

    return mstr_assign_cstr(dest, src->data);
}

mstr *
mstr_free(mstr *str)
{
    if (str == NULL)
        return NULL;
    free(str->data);
    return mstr_init(str);
}
