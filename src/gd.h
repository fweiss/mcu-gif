#pragma once

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#if defined(_MSC_VER)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} gd_color_t;

typedef uint8_t gd_index_t;

typedef struct {
    uint16_t width;
    uint16_t height;
    uint8_t globalColorTableFlag;
    uint16_t globalColorTableSize; // needs to handle 256
} gd_info_t;

typedef struct {
    uint8_t disposal_method;
} gd_graphic_control_extension_t;

typedef struct {
    uint16_t image_left;
    uint16_t image_top;
    uint16_t image_width;
    uint16_t image_height;
    size_t image_size;                // convenience
    uint16_t local_color_table_size;
    bool local_color_table;
    bool interlace;
    bool sort;
} gd_image_descriptor_t;

// todo internal?
typedef struct {
    uint32_t *colorTable;
    uint8_t *imageData;
} gd_decode_t;

typedef enum {
    GD_BLOCK_INITIAL = 0, // not sure if this is used
    GD_BLOCK_HEADER = 1,
    GD_BLOCK_LOGICAL_SCREEN_DESCRIPTOR = 2,
    GD_BLOCK_GLOBAL_COLOR_TABLE = 3,
    GD_BLOCK_GRAPHIC_CONTROL_EXTENSION = 4,
    GD_BLOCK_IMAGE_DESCRIPTOR = 5,
    GD_BLOCK_IMAGE_DATA = 6,
    GD_BLOCK_TRAILER = 7,
    GD_BLOCK_COMMENT_EXTENSION = 8,
    GD_BLOCK_PLAIN_TEXT_EXTENSION = 9,
    GD_BLOCK_APPLICATION_EXTENSION = 10,
    GD_BLOCK_LOGICAL_EOF = 11,
} gd_block_type_t;

typedef enum {
    GD_X_OK = 0, // conflict internal.h
    GD_ERR_BLOCK_PREFIX = 1,    // unexpected beginbing of block
    GD_ERR_EOF = 2,             // premature end of file reached
} gd_err_t;

typedef struct {
    void* fp; // actually opaque FILE
    ssize_t (*fread)(void* ptr, size_t size, size_t count, void* stream);
    gd_info_t info;
    gd_err_t err;
    gd_block_type_t next_block_type;
} gd_main_t;

gd_block_type_t gd_next_block_type(gd_main_t *main);

void gd_init(gd_main_t *main);
void gd_read_header(gd_main_t *main);
void gd_read_logical_screen_descriptor(gd_main_t *main, gd_info_t *info);
void gd_read_global_color_table(gd_main_t *main, gd_color_t *color_table);
void gd_read_graphic_control_extension(gd_main_t *main, gd_graphic_control_extension_t *gce);
void gd_read_image_descriptor(gd_main_t *main, gd_image_descriptor_t* imd);
void gd_read_image_data(gd_main_t *main, gd_index_t *output, size_t capacity);
void gd_read_trailer(gd_main_t *main);

// abstract file read
#define GD_READ(dp, ds) (main->fread((dp), 1, (ds), main->fp))
