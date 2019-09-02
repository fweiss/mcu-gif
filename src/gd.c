#include "gd.h"

#include "file.h"

#define LE(a, b) ((a) | (b << 8))

/* file system abstractions */

struct {
    long (*read)(int fd, char *buf, long count);
} gd_config;

#define READ(f, b, c) (gd_config.read(f, b, c))

/* decoder state */

static struct {
    int width;
} gd_state;

void gd_init(read_func_t read) {
    gd_config.read = read;
}

void gd_begin(int fd) {
    char header[13];
    long count = READ(fd, header, sizeof(header));
    gd_state.width = LE(header[6], header[7]);
}

void gd_info_get(gd_info_t *info) {
    info->status = 0;
    info->width = gd_state.width;
}

void gd_render_frame(gd_frame_t *frame) {
    frame->pixels[0] = 0x11223344;
    frame->status = 0;
}
