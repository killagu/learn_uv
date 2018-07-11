//
// Created by killa on 2018/6/28.
//

#ifndef UVSERVER_SERVER_H
#define UVSERVER_SERVER_H

#include <uv.h>

typedef struct http_server_s http_server_t;

struct http_server_s {
    uv_tcp_t tcp_server;
    uv_loop_t *loop;
    struct sockaddr_in addr;

    boolean_t listened;
};


int init_server(http_server_t *server, uv_loop_t *loop);

int bind_address(http_server_t *server, char *addr, unsigned int port);

int server_listen(http_server_t *server, int backlog, uv_connection_cb cb);

#endif //UVSERVER_SERVER_H
