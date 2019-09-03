#pragma once

#include <stdint.h>

typedef long (*read_func_t)(int fd, char *buf, long count);

typedef struct {
    int status;
    int width;
} gd_info_t;

typedef struct {
    int request;
    int status;
    uint16_t width;
    uint16_t height;
    uint32_t *pixels;
} gd_frame_t;

typedef enum {
    GD_OK,
    GD_BAD_SIGNATURE,
} gd_status_t;

void gd_init(read_func_t read);

void gd_begin(int fd);

void gd_info_get(gd_info_t *info);

void gd_render_frame(gd_frame_t *frame);
