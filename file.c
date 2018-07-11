//
// Created by killa on 2018/7/1.
//

#include "file.h"
#include <assert.h>
#include <uv.h>
#include "util.h"

void on_read(uv_fs_t *req) {
    file_request_t *request = CONTAINER_OF(req, file_request_t, read_req);
    if (req->result < 0) {
        fprintf(stderr, "Read error: %s\n", uv_strerror(req->result));
        free_uv_buffer(&request->buffer);
    } else if (req->result == 0) {
        uv_fs_t close_req;
        // synchronous
        uv_fs_close(request->loop, &close_req, req->file, NULL);
    } else if (req->result > 0) {
        uv_fs_t close_req;
        // synchronous
        uv_fs_close(request->loop, &close_req, req->file, NULL);
    }
    if (request->setting && request->setting->on_file_read) {
        request->setting->on_file_read(request);
    }
}

void on_file_stat(uv_fs_t *req) {
    file_request_t *request = CONTAINER_OF(req, file_request_t, stat_req);
    if (!(req->statbuf.st_mode & S_IFMT)) {
        fprintf(stderr, "error opening file: %s is not file\n", req->path);
        if (request->setting && request->setting->on_file_read) {
            request->setting->on_file_read(request);
        }
    } else {
        request->buffer.base = malloc(sizeof(char) * req->statbuf.st_size);
        request->buffer.len = req->statbuf.st_size;
        uv_fs_read(req->loop, &request->read_req, req->file,
                   &request->buffer, 1, -1, on_read);
    }
}

void on_open(uv_fs_t *req) {
    file_request_t *request = CONTAINER_OF(req, file_request_t, open_req);
    if (req->result >= 0) {
        uv_fs_fstat(request->loop, &request->stat_req, req->result, on_file_stat);
    } else {
        fprintf(stderr, "error opening file: %s\n", uv_strerror((int) req->result));
        if (request->setting && request->setting->on_file_read) {
            request->setting->on_file_read(request);
        }
    }
}

int read_file(file_request_t *request) {
    uv_fs_open(request->loop, &request->open_req, request->path, O_RDONLY, 0, on_open);
    return 0;
}

void create_file_request(
        uv_loop_t *loop,
        file_request_t *request,
        const char *path,
        file_request_setting_t *setting) {
    memset(request, 0, sizeof(request));
    request->loop = loop;
    request->path = path;
    request->buffer.base = NULL;
    request->buffer.len = 0;
    request->setting = setting;
}

void free_file_request(file_request_t *request) {
    uv_fs_req_cleanup(&request->open_req);
    uv_fs_req_cleanup(&request->read_req);
    uv_fs_req_cleanup(&request->write_req);
    uv_fs_req_cleanup(&request->stat_req);
    free_uv_buffer(&request->buffer);

    free(request->path);
    free(request->setting);
}
