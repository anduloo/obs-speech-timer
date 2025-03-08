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

#include "../util/c99defs.h"

#ifdef __cplusplus
extern "C" {
#endif

struct proc_handler;
typedef struct proc_handler proc_handler_t;

EXPORT proc_handler_t *proc_handler_create(void);
EXPORT void proc_handler_destroy(proc_handler_t *handler);

EXPORT void proc_handler_add(proc_handler_t *handler,
                           const char *decl_string,
                           void (*callback)(void *param,
                                          struct calldata *data),
                           void *param);

EXPORT bool proc_handler_call(proc_handler_t *handler,
                            const char *name,
                            struct calldata *params);

#ifdef __cplusplus
}
#endif 