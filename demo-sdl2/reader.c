#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <SDL.h>

#include "gd.h"

void renderPixels(SDL_Renderer *renderer, uint8_t *pixels, gd_color_t *colorTable);
void sketch(SDL_Renderer *renderer);

void drawGif(SDL_Renderer *renderer) {
    sketch(renderer);
}

void renderPixels(SDL_Renderer *renderer, gd_index_t *pixels, gd_color_t *colorTable) {
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
            gd_color_t c = colorTable[pixels[i]];
            uint8_t r = c.r;
            uint8_t g = c.g;
            uint8_t b = c.b;
            uint8_t a = 0xff;
            SDL_SetRenderDrawColor(renderer, r, g, b, a);
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

// based on gd returning the next block type read from the file
void sketch(SDL_Renderer *renderer) {
    gd_main_t main;
    gd_info_t info;

    // int fd = open("samples/sample_1.gif", O_RDONLY);
    // main.read = read;
    // main.fd = fd;

    FILE* fp = fopen("samples/sample_1.gif", "rb");
    main.fp = fp;
    main.fread = fread;

    gd_init(&main);
    static gd_color_t gct[4];

    const uint16_t pixels_size = 100;
    static gd_index_t pixels[100];
    gd_graphic_control_extension_t gce;

    int blockLimit = 100;
    for ( ; blockLimit>0; blockLimit--) {
        gd_block_type_t nextBlockType = gd_next_block_type(&main);
        printf("next block type %d\n", nextBlockType);
        switch (nextBlockType) {
            case GD_BLOCK_INITIAL: // no longer uaed
                break;
            case GD_BLOCK_HEADER:
                gd_read_header2(&main);
                break;
            case GD_BLOCK_LOGICAL_SCREEN_DESCRIPTOR:
                gd_read_logical_screen_descriptor(&main, &info);
                break;
            case GD_BLOCK_GLOBAL_COLOR_TABLE:
                // alloc
                gd_read_global_color_table(&main, gct);
                // check status
                break;
            case GD_BLOCK_GRAPHIC_CONTROL_EXTENSION:
                gd_read_graphic_control_extension(&main, &gce);
                break;
            case GD_BLOCK_IMAGE_DESCRIPTOR:
                gd_read_image_descriptor(&main);
                break;
            case GD_BLOCK_IMAGE_DATA:
                gd_read_image_data(&main, pixels, pixels_size);
                renderPixels(renderer, pixels, gct);
                break;
            case GD_BLOCK_TRAILER:
                printf("end of gif parsing\n");
                blockLimit = 0;
                break;
        }
    }
    if (blockLimit == 0) {
        printf("block limit expired");
    }
}
