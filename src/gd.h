#pragma once

#include <stdint.h>

typedef struct {
    int request;
    int status;
    uint16_t width;
    uint16_t height;
    uint32_t *pixels;
} gd_frame_t;

void gd_render_frame(gd_frame_t *frame);
