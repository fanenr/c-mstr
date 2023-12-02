#ifndef MSTR_H
#define MSTR_H

#include <stddef.h>

#define MSTR_INIT_CAP 8
#define MSTR_EXPAN_RATION 2

typedef struct
{
    size_t cap;
    size_t len;
    char *data;
} mstr;

extern mstr *mstr_init(mstr *str);

extern size_t mstr_cap(const mstr *str);

extern size_t mstr_len(const mstr *str);

extern const char *mstr_cstr(const mstr *str);

extern mstr *mstr_reserve(mstr *dest, size_t cap);

extern mstr *mstr_cat_char(mstr *dest, char src);

extern mstr *mstr_cat_cstr(mstr *dest, const char *src);

extern mstr *mstr_cat_mstr(mstr *dest, const mstr *src);

extern mstr *mstr_assign_cstr(mstr *dest, const char *src);

extern mstr *mstr_assign_mstr(mstr *dest, const mstr *src);

extern void mstr_free(mstr *str);

#endif
