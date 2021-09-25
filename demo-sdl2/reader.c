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
    gd_graphic_control_extension_t gce;
    gd_image_descriptor_t imd;
    // allocated dynamically
    gd_color_t* gct = 0;
    gd_index_t* pixels = 0;

    FILE* fp = fopen("samples/sample_1.gif", "rb");
    // FILE* fp = fopen("samples/128px-Dancing.gif", "rb");
    main.fp = fp;
    // signature mismatch, should be opaque (no FILE)
    main.fread = (void*)fread;

    gd_init(&main);

    int blockLimit = 100;
    for ( ; blockLimit>0; blockLimit--) {
        gd_block_type_t nextBlockType = gd_next_block_type(&main);
        printf("next block type %d\n", nextBlockType);
        switch (nextBlockType) {
            case GD_BLOCK_INITIAL: // no longer uaed
                break;
            case GD_BLOCK_HEADER:
                gd_read_header(&main);
                break;
            case GD_BLOCK_LOGICAL_SCREEN_DESCRIPTOR:
                gd_read_logical_screen_descriptor(&main, &info);
                break;
            case GD_BLOCK_GLOBAL_COLOR_TABLE:
                gct = (gd_color_t*)calloc(info.globalColorTableSize, sizeof(gd_color_t));
                gd_read_global_color_table(&main, gct);
                // check status
                break;
            case GD_BLOCK_GRAPHIC_CONTROL_EXTENSION:
                gd_read_graphic_control_extension(&main, &gce);
                break;
            case GD_BLOCK_IMAGE_DESCRIPTOR:
                gd_read_image_descriptor(&main, &imd);
                break;
            case GD_BLOCK_IMAGE_DATA:
                pixels = (gd_index_t*)calloc(imd.image_size, sizeof(gd_index_t));
                gd_read_image_data(&main, pixels, imd.image_size);
                renderPixels(renderer, pixels, gct);
                break;
            case GD_BLOCK_TRAILER:
                printf("end of gif parsing\n");
                blockLimit = 0;
                break;
            case GD_BLOCK_COMMENT_EXTENSION:
            case GD_BLOCK_PLAIN_TEXT_EXTENSION:
            case GD_BLOCK_APPLICATION_EXTENSION:
            case GD_BLOCK_LOGICAL_EOF:
                // ignored
                break;
        }
    }
    if (blockLimit == 0) {
        printf("block limit expired");
    }
    if (gct) {
        free(gct);
        gct = 0;
    }
    if (pixels) {
        free(pixels);
        pixels = 0;
    }
}
