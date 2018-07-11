//
// Created by killa on 2018/6/30.
//

#include "request.h"
#include "util.h"
#include "string.h"

#define my_memcpy(result, offset, protocol, length) \
    do { \
      memcpy(&result[offset], protocol, length); \
      offset += length; \
    } while(0)

#define my_buf_memcpy(result, offset, buf) \
    do { \
      memcpy(&result[offset], buf.base, buf.len); \
      offset += buf.len; \
    } while(0)

#define CLRF "\r\n"


//const char *res = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nhello,world!";
const char *workspace = "/Users/killa/Desktop/html";

void write_response(http_request_t *request);

void on_close(uv_handle_t *handle) {
    http_request_t *request = CONTAINER_OF(handle, http_request_t, client);
    free_http_request(request);
}

void free_http_request(http_request_t *request) {
    DEBUG_PRINT("free http request\n");
    reset_http_request(request);
    free(request);
}

void on_data(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) {
    http_request_t *request = CONTAINER_OF(client, http_request_t, client);
    if (nread > 0) {
        http_parser *parser = &request->parser;
        http_parser_settings *settings = &request->settings;
        http_parser_execute(parser, settings, buf->base, nread);
    }
    if (nread < 0) {
        if (nread != UV_EOF) {
            fprintf(stderr, "Read error %s\n", uv_err_name(nread));
        }
        uv_close((uv_handle_t *) client, on_close);
    }
    free(buf->base);
}

int on_url(http_parser *parser, const char *at, size_t length) {
    http_request_t *request = CONTAINER_OF(parser, http_request_t, parser);
    alloc_uv_buffer(&request->url, at, length);
    return 0;
}

//int on_status(http_parser *parser, const char *at, size_t length) {
//    http_request_t *request = CONTAINER_OF(parser, http_request_t, parser);
//    request->status = atoi(at);
//    return 0;
//}

int on_header_field(http_parser *parser, const char *at, size_t length) {
    http_request_t *request = CONTAINER_OF(parser, http_request_t, parser);
    header_t *header = (header_t *) malloc(sizeof(header_t));
    init_header(header);
    alloc_uv_buffer(&header->field, at, length);
    add_header(&request->req_headers, header);
//    printf("on header field: %s\n", header->field);
    return 0;
}

int on_header_value(http_parser *parser, const char *at, size_t length) {
    http_request_t *request = CONTAINER_OF(parser, http_request_t, parser);
    header_t *header = request->req_headers.tail;
    alloc_uv_buffer(&header->value, at, length);
//    printf("on header value: %s\n", header->value);
    return 0;
}

int on_body(http_parser *parser, const char *at, size_t length) {
    http_request_t *request = CONTAINER_OF(parser, http_request_t, parser);
    alloc_uv_buffer(&request->data, at, length);
    return 0;
}

int on_message_begin(http_parser *parser) {
    DEBUG_PRINT("message begin\n");
    http_request_t *request = CONTAINER_OF(parser, http_request_t, parser);
    reset_http_request(request);
    return 0;
}

void print_request(http_request_t *request) {
    return;
    printf("url: ");
    print_uv_buffer(&request->url);
    printf("\n");
    header_t *header = request->req_headers.head;
    while (header != NULL) {
        print_uv_buffer(&header->field);
        printf("=");
        print_uv_buffer(&header->value);
        printf("\n");
        header = header->next;
    }
    printf("data: ");
    print_uv_buffer(&request->data);
    printf("\n");
}

void write_callback(uv_write_t *req, int status) {
#ifdef DEBUG
    http_request_t *request = CONTAINER_OF(req, http_request_t, res);
    print_request(request);
#endif
    DEBUG_PRINT("write callback\n");
    if (status) {
        fprintf(stderr, "Write error: %s\n", uv_strerror(status));
    }
}

int on_message_complete(http_parser *parser) {
    http_request_t *request = CONTAINER_OF(parser, http_request_t, parser);

    size_t offset = 0;
    size_t file_path_length = request->url.len + 25;
    char *file_path = malloc(sizeof(char) * (file_path_length + 1));
    my_memcpy(file_path, offset, workspace, 25);
    my_buf_memcpy(file_path, offset, request->url);
    file_path[file_path_length] = '\0';

    alloc_uv_buffer(&request->body, "hello, world!", 13);

    request->status = HTTP_STATUS_OK;
    // add content length
    add_res_header(&request->res_headers, "Content-Length", 14, "13", 2);

    // build res data
    write_response(request);

    print_request(request);

    uv_write(
            &request->res,
            (uv_stream_t *) &request->client,
            &request->res_data,
            1,
            write_callback);

    return 0;
}

void write_response(http_request_t *request) {
    char *protocol = "HTTP/1.1 ";
    char status[5];
    sprintf(status, "%3d ", request->status);
    const char *status_message = http_status_str(request->status);
    size_t message_length = strlen(status_message);
    size_t length = 13 + message_length + 2;
    length += request->res_headers.data_length;
    length += 2 + request->body.len;
    char *result = malloc(sizeof(char) * length);
    size_t offset = 0;

    // construct status line
    // HTTP/1.1 200 OK\r\n
    my_memcpy(result, offset, protocol, 9);
    my_memcpy(result, offset, status, 4);
    my_memcpy(result, offset, status_message, message_length);
    my_memcpy(result, offset, CLRF, 2);
    // construct headers
    // Content-Length: 12\r\n
    header_t *header = request->res_headers.head;
    while (header != NULL) {
        my_buf_memcpy(result, offset, header->field);
        my_memcpy(result, offset, ": ", 2);
        my_buf_memcpy(result, offset, header->value);
        my_memcpy(result, offset, CLRF, 2);
        header = header->next;
    }
    // append \r\n after headers
    my_memcpy(result, offset, CLRF, 2);
    // append res body
    my_buf_memcpy(result, offset, request->body);
    request->res_data.base = result;
    request->res_data.len = length;
}

http_request_t *create_http_request(uv_loop_t *loop) {
    DEBUG_PRINT("create http request\n");
    http_request_t *request = (http_request_t *) malloc(sizeof(http_request_t));
    memset(request, 0, sizeof(*request));
    request->loop = loop;
    init_uv_buffer(&request->data);
    init_uv_buffer(&request->url);
    uv_tcp_init(loop, &request->client);

    http_parser_init(&request->parser, HTTP_REQUEST);
    http_parser_settings_init(&request->settings);
    http_parser_settings *settings = &request->settings;
    settings->on_url = on_url;
    settings->on_header_field = on_header_field;
    settings->on_header_value = on_header_value;
    settings->on_body = on_body;
    settings->on_message_complete = on_message_complete;
    settings->on_message_begin = on_message_begin;
    return request;
}

void reset_http_request(http_request_t *request) {
    DEBUG_PRINT("reset http request\n");
    free_uv_buffer(&request->data);
    free_uv_buffer(&request->url);
    free_uv_buffer(&request->body);
    free_uv_buffer(&request->res_data);
    free_header(&request->req_headers);
    free_header(&request->res_headers);
}

void add_res_header(header_link_t *link,
                    const char *field,
                    size_t field_length,
                    const char *value,
                    size_t value_length) {
    header_t *header = (header_t *) malloc(sizeof(header_t));
    init_header(header);
    alloc_uv_buffer(&header->field, field, field_length);
    alloc_uv_buffer(&header->value, value, value_length);
    add_header(link, header);
}