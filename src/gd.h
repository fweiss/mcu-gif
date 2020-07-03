#pragma once

#include <stdint.h>
#include <string.h>

typedef struct {
    long (*read)(int, uint8_t*, long);
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
    long (*read)(int, uint8_t*, long);
    int fd;
} gd_main_t;

// deprecated
void gd_open(gd_info_t *info);
// deprecated
void gd_decode(gd_decode_t *decode);

void gd_init(gd_main_t *main);
void gd_read_header(gd_main_t *main);

void gd_read_image(gd_main_t *main, uint16_t *output, size_t capacity);
