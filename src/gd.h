#pragma once

#include <stdint.h>

typedef long (*read_func_t)(int fd, uint8_t *buf, long count);

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} color_t;

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
    bool has_local_color_table;
    uint32_t *pixels;  // row-major array of pixel color table indices
} gd_frame_t;

typedef struct {
    uint16_t size;
    color_t *colors;
} gd_colortab_t;

typedef enum {
    GD_OK,
    GD_BAD_SIGNATURE,
} gd_status_t;

void gd_init(read_func_t read);

void gd_begin(int fd);

void gd_end();

void gd_info_get(gd_info_t *info);

void gd_render_frame(gd_frame_t *frame);

void gd_decode_lzw(uint16_t size, const uint8_t *encoded, uint8_t *decoded);

void gd_global_colortab_get(gd_colortab_t *colortab);

