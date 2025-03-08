#pragma once

#include <stdint.h>
#include <obs-module.h>
#include <obs-frontend-api.h>
#include <util/text-lookup.h>

#define MAX_AV_PLANES 8
#define MAX_AUDIO_CHANNELS 8
#define MAX_AUDIO_MIXES 6

#ifdef _MSC_VER
#define FORCE_INLINE __forceinline
#else
#define FORCE_INLINE __attribute__((always_inline))
#endif

#define UNUSED_PARAMETER(param) (void)param

#ifdef _WIN32
#define MODULE_EXPORT __declspec(dllexport)
#else
#define MODULE_EXPORT __attribute__((visibility("default")))
#endif

#define blog(level, msg, ...) \
	blog(level, "[speech-timer] " msg, ##__VA_ARGS__)

#define LOG_ERROR(format, ...) blog(LOG_ERROR, format, ##__VA_ARGS__)
#define LOG_WARNING(format, ...) blog(LOG_WARNING, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...) blog(LOG_INFO, format, ##__VA_ARGS__)
#define LOG_DEBUG(format, ...) blog(LOG_DEBUG, format, ##__VA_ARGS__)

typedef struct gs_effect gs_effect_t;
typedef struct video_output video_t;
typedef struct audio_output audio_t;
typedef struct audio_output_data audio_data;
typedef void (*audio_output_callback_t)(void *param, size_t mix_idx,
				      struct audio_data *data);
typedef struct signal_handler signal_handler_t;
typedef void (*signal_callback_t)(void *data, calldata_t *cd);
typedef struct profiler_name_store profiler_name_store_t;
typedef struct config_data config_t; 