#include <stdio.h>
#include <uv.h>
#include <stdlib.h>
#include "http_parser.h"
#include "server.h"
#include "util.h"
#include "request.h"

void on_connection_close(uv_handle_t *handle) {
    http_request_t *request = CONTAINER_OF(handle, http_request_t, client);
    free_http_request(request);
}

void alloc_data (uv_handle_t* handle,
                            size_t suggested_size,
                            uv_buf_t* buf) {
    buf->base = (char *) malloc(suggested_size);
    buf->len = suggested_size;
}


void on_new_connection(uv_stream_t *server, int status) {
    DEBUG_PRINT("on new connection\n");
    if (status < 0) {
        fprintf(stderr, "New connection error %s\n", uv_strerror(status));
        return;
    }
    http_server_t *http_server = CONTAINER_OF(server, http_server_t, tcp_server);
    http_request_t *request = create_http_request(http_server->loop);
    uv_tcp_t *client = &request->client;
    if (uv_accept(server, (uv_stream_t *) client) == 0) {
        uv_read_start((uv_stream_t *) client, alloc_data, on_data);
    } else {
        uv_close((uv_handle_t *) client, on_connection_close);
    }
}

int main() {
    uv_loop_t *loop = uv_default_loop();
    http_server_t *server = (http_server_t *) malloc(sizeof(http_server_t));
    int err;
    err = init_server(server, loop);
    if (err) {
        fprintf(stderr, "init server error: %s\n", uv_strerror(err));
        return 1;
    }
    err = bind_address(server, "0.0.0.0", 8000);
    if (err) {
        fprintf(stderr, "bind address error: %s\n", uv_strerror(err));
        return 1;
    }
    err = server_listen(server, 128, on_new_connection);
    if (err) {
        fprintf(stderr, "Listen error %s\n", uv_strerror(err));
        return 1;
    }
    return uv_run(loop, UV_RUN_DEFAULT);
}