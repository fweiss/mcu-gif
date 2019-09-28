#pragma once

#include <stdint.h>
#include <stdbool.h>

// todo allow configure either 8 bit or 16 bit color table indices

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
    uint16_t *pixels;  // row-major array of pixel color table indices
} gd_frame_t;

typedef struct {
    uint16_t size;
    color_t *colors;
} gd_colortab_t;

typedef enum {
    GD_OK,                      // successful operation
    GD_BAD_SIGNATURE,           // bad GIF file signature
    GD_SUB_BLOCK_SIZE,          // attempted to decode beyond sub block size
    GD_READ_END,                // end of stream reached before expected
    GD_BLOCK_NOT_FOUND,         // specific block type missing e.g. 0x2C
    GD_NULL_POINTER,            // required data pointer is null
} gd_status_t;

typedef struct {
    uint8_t minimum_code_size;
    uint8_t sub_block_size;
    uint8_t *sub_block;
    uint16_t *codes; // fixme s/b index stream
    uint16_t *code_count;
    gd_status_t status;
} gd_sub_block_decode_t;

void gd_init(read_func_t read);

void gd_begin(int fd);

void gd_end();

void gd_info_get(gd_info_t *info);

void gd_render_frame(gd_frame_t *frame);

void gd_sub_block_decode(gd_sub_block_decode_t *decode);

void gd_global_colortab_get(gd_colortab_t *colortab);

uint32_t gd_lookup_rgb(uint16_t index);

