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

enum audio_format {
    AUDIO_FORMAT_UNKNOWN,

    AUDIO_FORMAT_U8BIT,
    AUDIO_FORMAT_16BIT,
    AUDIO_FORMAT_32BIT,
    AUDIO_FORMAT_FLOAT,

    AUDIO_FORMAT_U8BIT_PLANAR,
    AUDIO_FORMAT_16BIT_PLANAR,
    AUDIO_FORMAT_32BIT_PLANAR,
    AUDIO_FORMAT_FLOAT_PLANAR,
};

enum speaker_layout {
    SPEAKERS_UNKNOWN,
    SPEAKERS_MONO,
    SPEAKERS_STEREO,
    SPEAKERS_2POINT1,
    SPEAKERS_4POINT0,
    SPEAKERS_4POINT1,
    SPEAKERS_5POINT1,
    SPEAKERS_7POINT1,
};

struct audio_data {
    uint8_t *data[8];
    uint32_t frames;
    uint64_t timestamp;
};

struct audio_output_data {
    float *data[8];
    uint32_t frames;
    uint64_t timestamp;
};

struct audio_convert_info {
    uint32_t samples_per_sec;
    enum audio_format format;
    enum speaker_layout speakers;
};

struct audio_output_info {
    const char *name;

    uint32_t samples_per_sec;
    enum audio_format format;
    enum speaker_layout speakers;
};

#ifdef __cplusplus
}
#endif 