#include "gd.h"

#include <string.h>

#define LE(a, b) ((a) | (b << 8))

/* file system abstractions */

struct {
    long (*read)(int fd, char *buf, long count);
} gd_config;

#define READ(f, b, c) (gd_config.read(f, b, c))

/* decoder state */

static struct {
    int status;
    int width;
    int height;
} gd_state;

/* api functions */

void gd_init(read_func_t read) {
    gd_config.read = read;
}

void gd_begin(int fd) {
    gd_state.status = GD_OK;

    char header[13];
    long count = READ(fd, header, sizeof(header));
    // todo check count
    bool is_not_sig87a = strncmp(header, "GIF87a", 6);
    bool is_not_sig89a = strncmp(header, "GIF89a", 6);
    if (is_not_sig87a && is_not_sig89a) {
        gd_state.status = GD_BAD_SIGNATURE;
    }
    gd_state.width = LE(header[6], header[7]);
    gd_state.height = LE(header[8], header[9]);
}

void gd_info_get(gd_info_t *info) {
    info->status = gd_state.status;
    info->width = gd_state.width;
    info->height = gd_state.height;
}

void gd_render_frame(gd_frame_t *frame) {
    frame->pixels[0] = 0x11223344;
    frame->status = 0;
}
