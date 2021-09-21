#pragma once

#include <stdint.h>
#include <string.h>

#if defined(_MSC_VER)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

typedef uint8_t gd_index_t;

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
} gd_block_type_t;

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} gd_color_t;

typedef struct {
    uint16_t width;
    uint16_t height;
    uint8_t globalColorTableFlag;
    uint8_t globalColorTableSize;
} gd_info_t;

typedef struct {
    uint8_t disposal_method;
} gd_graphic_control_extension_t;

typedef struct {
    uint32_t *colorTable;
    uint8_t *imageData;
} gd_decode_t;

typedef struct {
    void* fp; // actually opaque FILE
    ssize_t (*fread)(void* ptr, size_t size, size_t count, void* stream);
    gd_info_t info;
    gd_block_type_t next_block_type;
} gd_main_t;

// deprecated
void gd_open(gd_info_t *info);

void gd_init(gd_main_t *main);
gd_block_type_t gd_next_block_type(gd_main_t *main);

void gd_read_header(gd_main_t *main, gd_info_t *info);
void gd_read_header2(gd_main_t *main);
void gd_read_logical_screen_descriptor(gd_main_t *main, gd_info_t *info);
void gd_read_global_color_table(gd_main_t *main, gd_color_t *color_table);
void gd_read_graphic_control_extension(gd_main_t *main, gd_graphic_control_extension_t *gce);
void gd_read_image_descriptor(gd_main_t *main);
void gd_read_image_data(gd_main_t *main, gd_index_t *output, size_t capacity);

// temp hack
#define GD_READ(dp, ds) (main->fread((dp), 1, (ds), main->fp))
