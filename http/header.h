//
// Created by killa on 2018/6/30.
//

#ifndef UVSERVER_HEADER_H
#define UVSERVER_HEADER_H

#include <uv.h>
#include <stdlib.h>

typedef struct header_s header_t;
typedef struct header_link_s header_link_t;

struct header_s {
    uv_buf_t field;
    uv_buf_t value;
    header_t *next;
};

struct header_link_s {
    header_t *head;
    header_t *tail;
    size_t count;
    size_t data_length;
};

void init_header(header_t *header);

void add_header(header_link_t *link, header_t *header);

void free_header(header_link_t *link);
#endif //UVSERVER_HEADER_H
