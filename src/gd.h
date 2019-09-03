#pragma once

#include <stdint.h>

typedef long (*read_func_t)(int fd, char *buf, long count);

typedef struct {
    uint8_t status;        // current decoder status
    uint16_t width;         // image height
    uint16_t height;        // image height
    bool gct;          // global color table present
    uint8_t gctb;      // global color table bits
} gd_info_t;

typedef struct {
    uint8_t request;
    uint8_t status;
    uint16_t width;
    uint16_t height;
    uint32_t *pixels;  // row-major array of pixel color table indices
} gd_frame_t;

typedef enum {
    GD_OK,
    GD_BAD_SIGNATURE,
} gd_status_t;

void gd_init(read_func_t read);

void gd_begin(int fd);

void gd_info_get(gd_info_t *info);

void gd_render_frame(gd_frame_t *frame);
