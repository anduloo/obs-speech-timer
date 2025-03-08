/*
 * Copyright (c) 2013 Hugh Bailey <obs.jim@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#pragma once

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "c99defs.h"
#include "bmem.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Dynamic array.
 *
 * NOTE: Not type-safe when using directly.
 *       Specifying size per call with inline maximizes compiler optimization.
 *
 *       See DARRAY macro at the bottom of the file for slightly safer usage.
 */

#define DARRAY_INVALID ((size_t)-1)

struct darray {
    void *array;
    size_t num;
    size_t capacity;
};

static inline void darray_init(struct darray *dst)
{
    dst->array = NULL;
    dst->num = 0;
    dst->capacity = 0;
}

static inline void darray_free(struct darray *dst)
{
    bfree(dst->array);
    dst->array = NULL;
    dst->num = 0;
    dst->capacity = 0;
}

static inline size_t darray_alloc_size(const size_t element_size,
                                     const struct darray *da)
{
    return element_size * da->num;
}

static inline void *darray_item(const size_t element_size,
                              const struct darray *da, size_t idx)
{
    return (void *)(((uint8_t *)da->array) + element_size * idx);
}

static inline void *darray_end(const size_t element_size,
                             const struct darray *da)
{
    if (!da->num)
        return NULL;

    return darray_item(element_size, da, da->num - 1);
}

static inline void darray_reserve(const size_t element_size, struct darray *dst,
                                const size_t capacity)
{
    void *ptr;
    if (capacity == 0 || capacity <= dst->capacity)
        return;

    ptr = bmalloc(element_size * capacity);
    if (dst->num)
        memcpy(ptr, dst->array, element_size * dst->num);
    if (dst->array)
        bfree(dst->array);
    dst->array = ptr;
    dst->capacity = capacity;
}

static inline void darray_ensure_capacity(const size_t element_size,
                                        struct darray *dst,
                                        const size_t new_size)
{
    size_t new_capacity;
    void *ptr;

    if (new_size <= dst->capacity)
        return;

    new_capacity = dst->capacity * 2;
    if (new_capacity < new_size)
        new_capacity = new_size;

    ptr = bmalloc(element_size * new_capacity);
    if (dst->array) {
        if (dst->num)
            memcpy(ptr, dst->array, element_size * dst->num);
        bfree(dst->array);
    }
    dst->array = ptr;
    dst->capacity = new_capacity;
}

static inline void darray_resize(const size_t element_size, struct darray *dst,
                               const size_t size)
{
    int b_clear;
    size_t old_num;

    if (size == dst->num) {
        return;
    } else if (size == 0) {
        dst->num = 0;
        return;
    }

    b_clear = size > dst->num;
    old_num = dst->num;

    darray_ensure_capacity(element_size, dst, size);
    dst->num = size;

    if (b_clear)
        memset(darray_item(element_size, dst, old_num), 0,
               element_size * (dst->num - old_num));
}

static inline void darray_copy(const size_t element_size, struct darray *dst,
                             const struct darray *da)
{
    if (da->num == 0) {
        darray_free(dst);
    } else {
        darray_resize(element_size, dst, da->num);
        memcpy(dst->array, da->array, element_size * da->num);
    }
}

static inline void darray_copy_array(const size_t element_size,
                                   struct darray *dst, const void *array,
                                   const size_t num)
{
    darray_resize(element_size, dst, num);
    memcpy(dst->array, array, element_size * dst->num);
}

static inline void darray_move(struct darray *dst, struct darray *src)
{
    darray_free(dst);
    memcpy(dst, src, sizeof(struct darray));
    src->array = NULL;
    src->capacity = 0;
    src->num = 0;
}

static inline size_t darray_find(const size_t element_size,
                               const struct darray *da, const void *item,
                               const size_t idx)
{
    size_t i;

    assert(idx <= da->num);

    for (i = idx; i < da->num; i++) {
        void *compare = darray_item(element_size, da, i);
        if (memcmp(compare, item, element_size) == 0)
            return i;
    }

    return DARRAY_INVALID;
}

static inline size_t darray_push_back(const size_t element_size,
                                    struct darray *dst, const void *item)
{
    darray_ensure_capacity(element_size, dst, dst->num + 1);
    memcpy(darray_item(element_size, dst, dst->num), item, element_size);
    return dst->num++;
}

static inline void darray_insert(const size_t element_size, struct darray *dst,
                               const size_t idx, const void *item)
{
    void *new_item;
    size_t move_count;

    assert(idx <= dst->num);

    if (idx == dst->num) {
        darray_push_back(element_size, dst, item);
        return;
    }

    move_count = dst->num - idx;
    darray_ensure_capacity(element_size, dst, dst->num + 1);

    new_item = darray_item(element_size, dst, idx);

    memmove(darray_item(element_size, dst, idx + 1), new_item,
            element_size * move_count);
    memcpy(new_item, item, element_size);

    dst->num++;
}

static inline void darray_erase(const size_t element_size, struct darray *dst,
                              const size_t idx)
{
    assert(idx < dst->num);

    if (idx >= dst->num || !--dst->num)
        return;

    memmove(darray_item(element_size, dst, idx),
            darray_item(element_size, dst, idx + 1),
            element_size * (dst->num - idx));
}

static inline void darray_erase_item(const size_t element_size,
                                   struct darray *dst, const void *item)
{
    size_t idx = darray_find(element_size, dst, item, 0);
    if (idx != DARRAY_INVALID)
        darray_erase(element_size, dst, idx);
}

static inline void darray_erase_range(const size_t element_size,
                                    struct darray *dst, const size_t start,
                                    const size_t end)
{
    size_t count, move_count;

    assert(start <= dst->num);
    assert(end <= dst->num);
    assert(end > start);

    count = end - start;
    if (count == 1) {
        darray_erase(element_size, dst, start);
        return;
    } else if (count == dst->num) {
        dst->num = 0;
        return;
    }

    move_count = dst->num - end;
    if (move_count) {
        memmove(darray_item(element_size, dst, start),
                darray_item(element_size, dst, end),
                element_size * move_count);
    }

    dst->num -= count;
}

static inline void darray_pop_back(const size_t element_size,
                                 struct darray *dst)
{
    assert(dst->num != 0);

    if (dst->num)
        dst->num--;
}

static inline void darray_join(const size_t element_size, struct darray *dst,
                             struct darray *da)
{
    darray_ensure_capacity(element_size, dst, dst->num + da->num);
    memcpy(darray_item(element_size, dst, dst->num), da->array,
           element_size * da->num);
    dst->num += da->num;
}

static inline void darray_split(const size_t element_size, struct darray *dst1,
                              struct darray *dst2, const struct darray *da,
                              const size_t split_idx)
{
    struct darray temp;

    assert(split_idx <= da->num);

    darray_init(&temp);
    darray_copy(element_size, &temp, da);

    darray_free(dst1);
    darray_free(dst2);

    if (split_idx)
        darray_copy_array(element_size, dst1, temp.array,
                         split_idx);
    if (temp.num - split_idx)
        darray_copy_array(element_size, dst2,
                         darray_item(element_size, &temp,
                                   split_idx),
                         temp.num - split_idx);

    darray_free(&temp);
}

static inline void darray_move_item(const size_t element_size,
                                  struct darray *dst, const size_t from,
                                  const size_t to)
{
    void *temp, *p_from, *p_to;

    if (from == to)
        return;

    temp = malloc(element_size);
    p_from = darray_item(element_size, dst, from);
    p_to = darray_item(element_size, dst, to);

    memcpy(temp, p_from, element_size);

    if (to < from)
        memmove(darray_item(element_size, dst, to + 1), p_to,
                element_size * (from - to));
    else
        memmove(p_from, darray_item(element_size, dst, from + 1),
                element_size * (to - from));

    memcpy(p_to, temp, element_size);
    free(temp);
}

static inline void darray_swap(const size_t element_size, struct darray *dst,
                             const size_t a, const size_t b)
{
    void *temp, *a_ptr, *b_ptr;

    assert(a < dst->num);
    assert(b < dst->num);

    if (a == b)
        return;

    temp = malloc(element_size);
    a_ptr = darray_item(element_size, dst, a);
    b_ptr = darray_item(element_size, dst, b);
    memcpy(temp, a_ptr, element_size);
    memcpy(a_ptr, b_ptr, element_size);
    memcpy(b_ptr, temp, element_size);
    free(temp);
}

/*
 * Defines to make dynamic arrays more type-safe.
 * Note: Still not 100% type-safe but much better than using darray directly
 *       Makes it a little easier to use as well.
 *
 * General use:
 *   DARRAY(type) array_name;
 *   da_init(array_name);
 *   da_free(array_name);
 *
 * Time to use:
 *   DARRAY(struct foo) foos;
 *   struct foo foo = {0};
 *
 *   da_init(foos);
 *   da_push_back(foos, &foo); //or pass struct inline
 */

#define DARRAY(type)                     \
    union {                             \
        struct darray da;               \
        struct {                        \
            type *array;                \
            size_t num;                 \
            size_t capacity;            \
        };                             \
    }

#define da_init(v) darray_init(&v.da)

#define da_free(v) darray_free(&v.da)

#define da_alloc_size(v) (sizeof(*v.array) * v.num)

#define da_end(v) darray_end(sizeof(*v.array), &v.da)

#define da_reserve(v, capacity) \
    darray_reserve(sizeof(*v.array), &v.da, capacity)

#define da_resize(v, size) darray_resize(sizeof(*v.array), &v.da, size)

#define da_copy(dst, src) \
    darray_copy(sizeof(*dst.array), &dst.da, &src.da)

#define da_copy_array(dst, src_array, n) \
    darray_copy_array(sizeof(*dst.array), &dst.da, src_array, n)

#define da_move(dst, src) darray_move(&dst.da, &src.da)

#define da_find(v, item, idx) \
    darray_find(sizeof(*v.array), &v.da, item, idx)

#define da_push_back(v, item) darray_push_back(sizeof(*v.array), &v.da, item)

#define da_push_back_new(v) \
    darray_push_back_new(sizeof(*v.array), &v.da)

#define da_insert(v, idx, item) \
    darray_insert(sizeof(*v.array), &v.da, idx, item)

#define da_erase(v, idx) darray_erase(sizeof(*v.array), &v.da, idx)

#define da_erase_item(v, item) \
    darray_erase_item(sizeof(*v.array), &v.da, item)

#define da_erase_range(v, from, to) \
    darray_erase_range(sizeof(*v.array), &v.da, from, to)

#define da_pop_back(v) darray_pop_back(sizeof(*v.array), &v.da)

#define da_join(dst, src) \
    darray_join(sizeof(*dst.array), &dst.da, &src.da)

#define da_split(dst1, dst2, src, idx) \
    darray_split(sizeof(*src.array), &dst1.da, &dst2.da, &src.da, idx)

#define da_move_item(v, from, to) \
    darray_move_item(sizeof(*v.array), &v.da, from, to)

#define da_swap(v, idx1, idx2) \
    darray_swap(sizeof(*v.array), &v.da, idx1, idx2)

#ifdef __cplusplus
}
#endif 