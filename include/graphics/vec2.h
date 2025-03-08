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

#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

struct vec2 {
    union {
        struct {
            float x, y;
        };
        float ptr[2];
    };
};

static inline void vec2_zero(struct vec2 *v)
{
    v->x = 0.0f;
    v->y = 0.0f;
}

static inline void vec2_set(struct vec2 *dst, float x, float y)
{
    dst->x = x;
    dst->y = y;
}

static inline void vec2_copy(struct vec2 *dst, const struct vec2 *v)
{
    dst->x = v->x;
    dst->y = v->y;
}

static inline void vec2_add(struct vec2 *dst, const struct vec2 *v1,
                          const struct vec2 *v2)
{
    dst->x = v1->x + v2->x;
    dst->y = v1->y + v2->y;
}

static inline void vec2_sub(struct vec2 *dst, const struct vec2 *v1,
                          const struct vec2 *v2)
{
    dst->x = v1->x - v2->x;
    dst->y = v1->y - v2->y;
}

static inline void vec2_mul(struct vec2 *dst, const struct vec2 *v1,
                          const struct vec2 *v2)
{
    dst->x = v1->x * v2->x;
    dst->y = v1->y * v2->y;
}

static inline void vec2_div(struct vec2 *dst, const struct vec2 *v1,
                          const struct vec2 *v2)
{
    dst->x = v1->x / v2->x;
    dst->y = v1->y / v2->y;
}

static inline void vec2_addf(struct vec2 *dst, const struct vec2 *v,
                           float f)
{
    dst->x = v->x + f;
    dst->y = v->y + f;
}

static inline void vec2_subf(struct vec2 *dst, const struct vec2 *v,
                           float f)
{
    dst->x = v->x - f;
    dst->y = v->y - f;
}

static inline void vec2_mulf(struct vec2 *dst, const struct vec2 *v,
                           float f)
{
    dst->x = v->x * f;
    dst->y = v->y * f;
}

static inline void vec2_divf(struct vec2 *dst, const struct vec2 *v,
                           float f)
{
    dst->x = v->x / f;
    dst->y = v->y / f;
}

static inline float vec2_len(const struct vec2 *v)
{
    return sqrtf(v->x * v->x + v->y * v->y);
}

static inline float vec2_dot(const struct vec2 *v1, const struct vec2 *v2)
{
    return v1->x * v2->x + v1->y * v2->y;
}

static inline void vec2_norm(struct vec2 *dst, const struct vec2 *v)
{
    float len = vec2_len(v);
    if (len > 0.0f) {
        len = 1.0f / len;
        dst->x = v->x * len;
        dst->y = v->y * len;
    }
}

static inline bool vec2_close(const struct vec2 *v1, const struct vec2 *v2,
                            float epsilon)
{
    return fabsf(v1->x - v2->x) <= epsilon &&
           fabsf(v1->y - v2->y) <= epsilon;
}

static inline void vec2_min(struct vec2 *dst, const struct vec2 *v1,
                          const struct vec2 *v2)
{
    dst->x = (v1->x < v2->x) ? v1->x : v2->x;
    dst->y = (v1->y < v2->y) ? v1->y : v2->y;
}

static inline void vec2_max(struct vec2 *dst, const struct vec2 *v1,
                          const struct vec2 *v2)
{
    dst->x = (v1->x > v2->x) ? v1->x : v2->x;
    dst->y = (v1->y > v2->y) ? v1->y : v2->y;
}

static inline void vec2_abs(struct vec2 *dst, const struct vec2 *v)
{
    dst->x = fabsf(v->x);
    dst->y = fabsf(v->y);
}

static inline void vec2_floor(struct vec2 *dst, const struct vec2 *v)
{
    dst->x = floorf(v->x);
    dst->y = floorf(v->y);
}

static inline void vec2_ceil(struct vec2 *dst, const struct vec2 *v)
{
    dst->x = ceilf(v->x);
    dst->y = ceilf(v->y);
}

#ifdef __cplusplus
}
#endif 