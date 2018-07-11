//
// Created by killa on 2018/6/30.
//

#ifndef UVSERVER_UTIL_H
#define UVSERVER_UTIL_H

#include <uv.h>
#include <stdlib.h>

#ifdef DEBUG
# define DEBUG_PRINT(x) printf x
#else
# define DEBUG_PRINT(x) do {} while (0)
#endif

#define CONTAINER_OF(ptr, type, field)                                        \
  ((type *) ((char *) (ptr) - ((char *) &((type *) 0)->field)))

static int alloc_uv_buffer(uv_buf_t *buf, const char *data, size_t suggested_size) {
    buf->base = (char *) malloc(suggested_size);
    memcpy(buf->base, data, suggested_size);
    buf->len = suggested_size;
    return 0;
}

static void init_uv_buffer(uv_buf_t *buf) {
    buf->base = NULL;
    buf->len = 0;
}

static void free_uv_buffer(uv_buf_t *buf) {
    free(buf->base);
    buf->base = NULL;
    buf->len = 0;
}

static void print_uv_buffer(uv_buf_t *buf) {
    for (size_t i = 0; i < buf->len; ++i) {
        putchar(buf->base[i]);
    }
}

#endif //UVSERVER_UTIL_H
