#include "gd.h"

#include <string.h>
#include <stdlib.h>

#define LE(a, b) ((a) | (b << 8))

/* file system abstractions */

struct {
    long (*read)(int fd, uint8_t *buf, long count);
} gd_config;

#define READ(f, b, c) (gd_config.read(f, b, c))

/* decoder state */

static struct {
    int status;
    int width;
    int height;
    bool gct;
    uint8_t gctb;
    color_t *gctf;
} gd_state;

/* api functions */

void gd_init(read_func_t read) {
    gd_config.read = read;
}

void gd_begin(int fd) {
    gd_state.status = GD_OK;

    uint8_t header[13];
    long count = READ(fd, header, sizeof(header));
    // todo check count
    bool is_not_sig87a = strncmp((char*)header, "GIF87a", 6);
    bool is_not_sig89a = strncmp((char*)header, "GIF89a", 6);
    if (is_not_sig87a && is_not_sig89a) {
        gd_state.status = GD_BAD_SIGNATURE;
    }
    gd_state.width = LE(header[6], header[7]);
    gd_state.height = LE(header[8], header[9]);

    gd_state.gct = header[10] & 0x80;
    gd_state.gctb = (header[10] & 0x03) + 1;
    if (gd_state.gct) {
        const uint16_t count = (1 << gd_state.gctb);
        const uint16_t size = count * sizeof(uint32_t);
        gd_state.gctf = (color_t*)malloc(size);
        long read = READ(fd, (uint8_t*)gd_state.gctf, size);
        if (read == 0) {
            gd_state.status = 22;
        }
    }
}

void gd_info_get(gd_info_t *info) {
    info->status = gd_state.status;
    info->width = gd_state.width;
    info->height = gd_state.height;
    info->gct = gd_state.gct;
    info->gctb = gd_state.gctb;
}

void gd_render_frame(gd_frame_t *frame) {
    frame->pixels[0] = 0x11223344;
    frame->status = 0;
}

void gd_decode_lzw(uint16_t size, const uint8_t *encoded, uint8_t *decoded) {
    decoded[0] = 0x99;
}

void gd_global_colortab_get(gd_colortab_t *colortab) {
    colortab->size = 1 << (gd_state.gctb + 0); // parser added 1
    colortab->colors = gd_state.gctf;
}

