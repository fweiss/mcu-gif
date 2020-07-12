#pragma once

#include <stdint.h>
#include <string.h>

typedef uint8_t gd_index_t;

typedef enum {
    GD_BLOCK_INITIAL = 0,
    GD_BLOCK_GLOBAL_COLOR_TABLE = 1,
    GD_BLOCK_GRAPHIC_CONTROL_EXTENSION = 2,
} gd_block_type_t;

typedef struct {
    uint16_t width;
    uint16_t height;
    uint8_t globalColorTableFlag;
    uint8_t globalColorTableSize;
} gd_info_t;

typedef struct {
    uint32_t *colorTable;
    uint8_t *imageData;
} gd_decode_t;

typedef struct {
    ssize_t (*read)(int, void*, size_t);
    int fd;
    gd_block_type_t next_block_type;
} gd_main_t;

// deprecated
void gd_open(gd_info_t *info);

void gd_init(gd_main_t *main);
gd_block_type_t gd_next_block_type(gd_main_t *main);

void gd_read_header(gd_main_t *main, gd_info_t *info);
void gd_read_global_color_table(gd_main_t *main, uint8_t *color_table);
void gd_read_image(gd_main_t *main, gd_index_t *output, size_t capacity);
