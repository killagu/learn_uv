//
// Created by killa on 2018/6/28.
//

#include <stdlib.h>
#include "server.h"


#define DEFAULT_PORT 8080
#define DEFAULT_BACKLOG 128


int init_server(http_server_t *server, uv_loop_t *loop) {
    memset(server, 0, sizeof(http_server_t));
    server->loop = loop;
    server->listened = FALSE;
    return uv_tcp_init(server->loop, &server->tcp_server);
}

int bind_address(http_server_t *server, char *addr, unsigned int port) {
    int err;
    err = uv_ip4_addr(addr, port, &server->addr);
    if (err != 0) {
        return err;
    }
    err = uv_tcp_bind(&server->tcp_server, (const struct sockaddr *) &server->addr, 0);
    return err;
}

int server_listen(http_server_t *server, int backlog, uv_connection_cb cb) {
    return uv_listen((uv_stream_t *) &server->tcp_server, backlog, cb);
}
