//
// Created by killa on 2018/7/1.
//

#ifndef UVSERVER_FILE_H
#define UVSERVER_FILE_H

#include <uv.h>

typedef struct file_request_s file_request_t;
typedef struct file_request_setting_s file_request_setting_t;

typedef void (*file_request_cb)(file_request_t *reque);

struct file_request_s {
    const char *path;
    uv_fs_t open_req;
    uv_fs_t read_req;
    uv_fs_t write_req;
    uv_fs_t stat_req;
    uv_loop_t *loop;
    uv_buf_t buffer;
    file_request_setting_t *setting;
};

struct file_request_setting_s {
    file_request_cb on_file_read;
};

void create_file_request(
        uv_loop_t *loop,
        file_request_t *request,
        const char *path,
        file_request_setting_t *setting);

void free_file_request(file_request_t *request);

int read_file(file_request_t *request);

#endif //UVSERVER_FILE_H
