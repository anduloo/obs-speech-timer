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

struct vec3 {
    union {
        struct {
            float x, y, z;
        };
        float ptr[3];
    };
};

static inline void vec3_zero(struct vec3 *v)
{
    v->x = 0.0f;
    v->y = 0.0f;
    v->z = 0.0f;
}

static inline void vec3_set(struct vec3 *dst, float x, float y, float z)
{
    dst->x = x;
    dst->y = y;
    dst->z = z;
}

static inline void vec3_copy(struct vec3 *dst, const struct vec3 *v)
{
    dst->x = v->x;
    dst->y = v->y;
    dst->z = v->z;
}

static inline void vec3_add(struct vec3 *dst, const struct vec3 *v1,
                          const struct vec3 *v2)
{
    dst->x = v1->x + v2->x;
    dst->y = v1->y + v2->y;
    dst->z = v1->z + v2->z;
}

static inline void vec3_sub(struct vec3 *dst, const struct vec3 *v1,
                          const struct vec3 *v2)
{
    dst->x = v1->x - v2->x;
    dst->y = v1->y - v2->y;
    dst->z = v1->z - v2->z;
}

static inline void vec3_mul(struct vec3 *dst, const struct vec3 *v1,
                          const struct vec3 *v2)
{
    dst->x = v1->x * v2->x;
    dst->y = v1->y * v2->y;
    dst->z = v1->z * v2->z;
}

static inline void vec3_div(struct vec3 *dst, const struct vec3 *v1,
                          const struct vec3 *v2)
{
    dst->x = v1->x / v2->x;
    dst->y = v1->y / v2->y;
    dst->z = v1->z / v2->z;
}

static inline void vec3_addf(struct vec3 *dst, const struct vec3 *v,
                           float f)
{
    dst->x = v->x + f;
    dst->y = v->y + f;
    dst->z = v->z + f;
}

static inline void vec3_subf(struct vec3 *dst, const struct vec3 *v,
                           float f)
{
    dst->x = v->x - f;
    dst->y = v->y - f;
    dst->z = v->z - f;
}

static inline void vec3_mulf(struct vec3 *dst, const struct vec3 *v,
                           float f)
{
    dst->x = v->x * f;
    dst->y = v->y * f;
    dst->z = v->z * f;
}

static inline void vec3_divf(struct vec3 *dst, const struct vec3 *v,
                           float f)
{
    dst->x = v->x / f;
    dst->y = v->y / f;
    dst->z = v->z / f;
}

static inline float vec3_len(const struct vec3 *v)
{
    return sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
}

static inline float vec3_dot(const struct vec3 *v1, const struct vec3 *v2)
{
    return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
}

static inline void vec3_cross(struct vec3 *dst, const struct vec3 *v1,
                            const struct vec3 *v2)
{
    dst->x = v1->y * v2->z - v1->z * v2->y;
    dst->y = v1->z * v2->x - v1->x * v2->z;
    dst->z = v1->x * v2->y - v1->y * v2->x;
}

static inline void vec3_norm(struct vec3 *dst, const struct vec3 *v)
{
    float len = vec3_len(v);
    if (len > 0.0f) {
        len = 1.0f / len;
        dst->x = v->x * len;
        dst->y = v->y * len;
        dst->z = v->z * len;
    }
}

static inline bool vec3_close(const struct vec3 *v1, const struct vec3 *v2,
                            float epsilon)
{
    return fabsf(v1->x - v2->x) <= epsilon &&
           fabsf(v1->y - v2->y) <= epsilon &&
           fabsf(v1->z - v2->z) <= epsilon;
}

static inline void vec3_min(struct vec3 *dst, const struct vec3 *v1,
                          const struct vec3 *v2)
{
    dst->x = (v1->x < v2->x) ? v1->x : v2->x;
    dst->y = (v1->y < v2->y) ? v1->y : v2->y;
    dst->z = (v1->z < v2->z) ? v1->z : v2->z;
}

static inline void vec3_max(struct vec3 *dst, const struct vec3 *v1,
                          const struct vec3 *v2)
{
    dst->x = (v1->x > v2->x) ? v1->x : v2->x;
    dst->y = (v1->y > v2->y) ? v1->y : v2->y;
    dst->z = (v1->z > v2->z) ? v1->z : v2->z;
}

static inline void vec3_abs(struct vec3 *dst, const struct vec3 *v)
{
    dst->x = fabsf(v->x);
    dst->y = fabsf(v->y);
    dst->z = fabsf(v->z);
}

static inline void vec3_floor(struct vec3 *dst, const struct vec3 *v)
{
    dst->x = floorf(v->x);
    dst->y = floorf(v->y);
    dst->z = floorf(v->z);
}

static inline void vec3_ceil(struct vec3 *dst, const struct vec3 *v)
{
    dst->x = ceilf(v->x);
    dst->y = ceilf(v->y);
    dst->z = ceilf(v->z);
}

#ifdef __cplusplus
}
#endif 