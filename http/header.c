//
// Created by killa on 2018/6/30.
//

#include "header.h"
#include "../util.h"

void init_header(header_t *header) {
    init_uv_buffer(&header->field);
    init_uv_buffer(&header->value);
    header->next = NULL;
}

void add_header(header_link_t *link, header_t *header) {
    if (link->head == NULL) {
        link->head = header;
        link->tail = header;
    } else {
        link->tail->next = header;
        link->tail = header;
    }
    ++link->count;
    link->data_length += header->field.len + 2 + header->value.len + 2;
}

void free_header(header_link_t *link) {
    header_t *temp = link->head;
    while (temp != NULL) {
        header_t *next = temp->next;
        free_uv_buffer(&temp->value);
        free_uv_buffer(&temp->field);
        free(temp);
        temp = next;
    }
    link->head = NULL;
    link->tail = NULL;
    link->count = 0;
    link->data_length = 0;
}