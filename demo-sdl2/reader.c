#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <SDL.h>

#include "gd.h"

/*
void reader() {
    char buf[] = {
0x47, 0x49, 0x46, 0x38, 0x39, 0x61, 0x0A, 0x00, 0x0A, 0x00, 0x91, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x21, 0xF9, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2C, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x0A, 0x00, 0x00, 0x02, 0x16, 0x8C, 0x2D, 0x99, 0x87, 0x2A, 0x1C, 0xDC, 0x33, 0xA0, 0x02, 0x75, 0xEC, 0x95, 0xFA, 0xA8, 0xDE, 0x60, 0x8C, 0x04, 0x91, 0x4C, 0x01, 0x00, 0x3B
    };
    int fd = open("sample_1.gif", O_RDONLY);
    printf("fd: %d\n", fd);

//    unsigned char b[2];
//    size_t c = read(fd, buf, sizeof(buf));
//    if (c == -1) {
//        exit(1);
//    }
//    printf("size: %l\n", c);
//    printf("b: %x\n", b[0]);
    gd_init(read);
    gd_begin(fd);
    gd_info_t info;
    gd_info_get(&info);
    printf("info status %d\n", info.status);
    gd_colortab_t colortab;
    gd_global_colortab_get(&colortab);
    printf("color tab size %d\n", colortab.size);


    uint16_t ipixels[1024];
    gd_frame_t frame;
    frame.pixels = ipixels;
    gd_render_frame(&frame);
    printf("render frame: istatus: %d\n", frame.status);
    printf("pixels 0. 9, 49: %d %d %d", ipixels[0], ipixels[4], ipixels[49]);
}
*/

void drawGif(SDL_Renderer *renderer) {
    static gd_index_t pix[100] = {0};

    const char *gifPath = "../demo-sdl2/sample_1.gif";

    int fd = open(gifPath, O_RDONLY);
    printf("fd: %d\n", fd);
    if (fd < 0) {
    	printf("error opening file: %s\n", gifPath);
    	exit(0);
    }

    gd_main_t main;
    gd_info_t info;
    main.read = read;
    main.fd = fd;

    gd_init(&main);
    gd_read_header(&main, &info);
//            gd_read_global_color_table(&main, colors);
//            gd read_graphic_control_extension(&main);
//            gd_read_image_descriptor(&main);
    gd_read_image(&main, pix, sizeof(pix));
//            gd_read_trailer(&main);


    static uint32_t pct[4] = { 0xffffffff, 0xff000000, 0x0000ffff, 0x000000ff };
    SDL_Rect rect;
    rect.x = 50;
    rect.y = 50;
    rect.w = 10;
    rect.h = 10;

    SDL_RenderClear(renderer);
    for (uint8_t y=0; y<10; y++) {
        for (uint8_t x=0; x<10; x++) {
            rect.x = x * 10 + 100;
            rect.y = y * 10 + 100;
            uint16_t i = y * 10 + x;
            uint32_t c = pct[pix[i]];
            uint8_t r = (c >> 24) & 0xff;;
            uint8_t g = (c >> 16) & 0xff;
            uint8_t b = (c >> 8) & 0xff;
            uint8_t a = c & 0xff;
            SDL_SetRenderDrawColor(renderer, r, g, b, a);
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

