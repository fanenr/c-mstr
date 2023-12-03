#ifndef MSTR_H
#define MSTR_H

#include <stddef.h>

#define MSTR_INIT_CAP 8
#define MSTR_EXPAN_RATIO 2

typedef struct
{
    size_t cap;
    size_t len;
    char *data;
} mstr;

extern mstr *mstr_init(mstr *str);
extern mstr *mstr_free(mstr *str);
extern mstr *mstr_reserve(mstr *dest, size_t cap);

static inline mstr mstr_new(void);
static inline mstr mstr_new_cap(size_t cap);
static inline mstr mstr_new_char(char src);
static inline mstr mstr_new_cstr(const char *src);

extern size_t mstr_cap(const mstr *str);
extern size_t mstr_len(const mstr *str);
extern const char *mstr_data(const mstr *str);

extern char *mstr_move_cstr(mstr *str);
extern mstr *mstr_move_mstr(mstr *dest, mstr *src);

extern mstr *mstr_cat_char(mstr *dest, char src);
extern mstr *mstr_cat_cstr(mstr *dest, const char *src);
extern mstr *mstr_cat_mstr(mstr *dest, const mstr *src);

extern mstr *mstr_assign_char(mstr *dest, char src);
extern mstr *mstr_assign_cstr(mstr *dest, const char *src);
extern mstr *mstr_assign_mstr(mstr *dest, const mstr *src);

static inline mstr mstr_new(void)
{
    mstr str;
    mstr_init(&str);
    return str;
}

static inline mstr mstr_new_cap(size_t cap)
{
    mstr str;
    mstr_init(&str);
    mstr_reserve(&str, cap);
    return str;
}

static inline mstr mstr_new_char(char src)
{
    mstr str;
    mstr_init(&str);
    mstr_assign_char(&str, src);
    return str;
}

static inline mstr mstr_new_cstr(const char *src)
{
    mstr str;
    mstr_init(&str);
    mstr_assign_cstr(&str, src);
    return str;
}

#endif
