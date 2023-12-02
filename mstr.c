#include "mstr.h"
#include <stdlib.h>
#include <string.h>

mstr *mstr_init(mstr *str)
{
    str->data = NULL;
    str->len = str->cap = 0;
    return str;
}

size_t mstr_cap(const mstr *str)
{
    return str->cap;
}

size_t mstr_len(const mstr *str)
{
    return str->len;
}

const char *mstr_cstr(const mstr *str)
{
    return str->data;
}

mstr *mstr_reserve(mstr *dest, size_t cap)
{
    if (cap <= dest->cap)
        return dest;

    char *data = realloc(dest->data, cap);
    if (data == NULL)
        return NULL;

    dest->cap = cap;
    dest->data = data;
    return dest;
}

mstr *mstr_cat_char(mstr *dest, char src)
{
    if (src == '\0')
        return dest;

    if (dest->cap < dest->len + 2) {
        size_t cap = MSTR_INIT_CAP;
        if (dest->cap > 0)
            cap = dest->cap * MSTR_EXPAN_RATIO;
        if (mstr_reserve(dest, cap) != dest)
            return NULL;
    }

    dest->data[dest->len++] = src;
    dest->data[dest->len] = '\0';
    return dest;
}

mstr *mstr_cat_cstr(mstr *dest, const char *src)
{
    if (src == NULL)
        return dest;

    if (*src == '\0')
        return dest;

    size_t cap = dest->cap;
    size_t dest_len = dest->len;
    size_t src_len = strlen(src);
    if (cap < dest_len + src_len + 1) {
        cap = src_len + 1;
        if (dest->cap > 0)
            cap += dest->len;
        if (mstr_reserve(dest, cap) != dest)
            return NULL;
    }

    char *pos = dest->data + dest_len;
    char *move = memmove(pos, src, src_len);
    if (move != pos)
        return NULL;

    dest->len += src_len;
    dest->data[dest_len] = '\0';
    return dest;
}

mstr *mstr_cat_mstr(mstr *dest, const mstr *src)
{
    if (src == NULL)
        return dest;
    return mstr_cat_cstr(dest, src->data);
}

mstr *mstr_assign_cstr(mstr *dest, const char *src)
{
    if (src == NULL) {
        mstr_free(dest);
        return dest;
    }

    if (*src == '\0') {
        mstr_free(dest);
        return dest;
    }

    size_t src_len = strlen(src);
    size_t cap = src_len + 1;

    char *data = malloc(cap);
    if (data == NULL)
        return NULL;

    char *move = memmove(data, src, cap);
    if (move != data) {
        free(data);
        return NULL;
    }

    free(dest->data);
    data[src_len] = '\0';
    dest->len = src_len;
    dest->data = data;
    dest->cap = cap;
    return dest;
}

mstr *mstr_assign_mstr(mstr *dest, const mstr *src)
{
    if (src == NULL) {
        mstr_free(dest);
        return dest;
    }

    return mstr_assign_cstr(dest, src->data);
}

void mstr_free(mstr *str)
{
    free(str->data);
    mstr_init(str);
}
