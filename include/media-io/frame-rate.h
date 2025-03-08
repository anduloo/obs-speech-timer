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

#include <stdint.h>
#include "../util/c99defs.h"

#ifdef __cplusplus
extern "C" {
#endif

struct media_frames_per_second {
    uint32_t numerator;
    uint32_t denominator;
};

static inline double media_frames_per_second_to_frame_interval(
    struct media_frames_per_second fps)
{
    return (double)fps.denominator / (double)fps.numerator;
}

static inline double media_frames_per_second_to_fps(
    struct media_frames_per_second fps)
{
    return (double)fps.numerator / (double)fps.denominator;
}

static inline bool media_frames_per_second_is_valid(
    struct media_frames_per_second fps)
{
    return fps.numerator && fps.denominator;
}

#ifdef __cplusplus
}
#endif 