//
// Created by killa on 2018/6/30.
//

#ifndef UVSERVER_REQUEST_H
#define UVSERVER_REQUEST_H

#include <stdlib.h>
#include <uv.h>
#include "http_parser.h"
#include "http/header.h"

typedef struct http_request_s http_request_t;

struct http_request_s {
    //region request
    uv_buf_t url;
    uv_buf_t data;
    uv_tcp_t client;
    header_link_t req_headers;
    //endregion

    //region response
    enum http_status status;
    header_link_t res_headers;
    uv_buf_t body;
    uv_write_t res;

    uv_buf_t res_data;
    //endregion

    //region http parser
    http_parser parser;
    http_parser_settings settings;
    //endregion

    uv_loop_t *loop;
};


http_request_t *create_http_request(uv_loop_t *loop);

void reset_http_request(http_request_t *request);

void free_http_request(http_request_t *request);

void on_data(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf);

void print_request(http_request_t *request);

void add_res_header(header_link_t *link,
                    const char *field,
                    size_t field_length,
                    const char *value,
                    size_t value_length);

#endif //UVSERVER_REQUEST_H
