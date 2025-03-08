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
#include <stdbool.h>
#include "../util/c99defs.h"

#ifdef __cplusplus
extern "C" {
#endif

struct gs_texture;
struct gs_sampler_state;
struct gs_shader;
struct gs_swap_chain;
struct gs_texture_render_target;
struct gs_zstencil_buffer;
struct gs_shader_param;
struct gs_device;
struct gs_stage_surface;
struct gs_image_file;

typedef struct gs_texture gs_texture_t;
typedef struct gs_sampler_state gs_sampler_state_t;
typedef struct gs_shader gs_shader_t;
typedef struct gs_swap_chain gs_swap_chain_t;
typedef struct gs_texture_render_target gs_texrender_t;
typedef struct gs_zstencil_buffer gs_zstencil_t;
typedef struct gs_shader_param gs_sparam_t;
typedef struct gs_device gs_device_t;
typedef struct gs_stage_surface gs_stagesurf_t;
typedef struct gs_image_file gs_image_file_t;

enum gs_color_format {
    GS_UNKNOWN,
    GS_A8,
    GS_R8,
    GS_RGBA,
    GS_BGRX,
    GS_BGRA,
    GS_R10G10B10A2,
    GS_RGBA16,
    GS_R16,
    GS_RGBA16F,
    GS_RGBA32F,
    GS_RG16F,
    GS_RG32F,
    GS_R16F,
    GS_R32F,
    GS_DXT1,
    GS_DXT3,
    GS_DXT5,
};

enum gs_zstencil_format {
    GS_ZS_NONE,
    GS_Z16,
    GS_Z24_S8,
    GS_Z32F,
    GS_Z32F_S8X24,
};

enum gs_color_space {
    GS_CS_SRGB,
    GS_CS_SRGB_16F,
    GS_CS_709_SCRGB,
    GS_CS_709_EXTENDED,
};

#ifdef __cplusplus
}
#endif 