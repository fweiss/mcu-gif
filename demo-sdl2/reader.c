#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
// #include <unistd.h>
#include "windows.h"

// windows
#include "io.h"
#pragma warning(disable : 4996)
#define open _open

#include <SDL.h>

#include "gd.h"

typedef struct {
    uint16_t width;
    uint16_t height;
    gd_color_t* colors;
    gd_index_t* pixels;
    size_t zoom;
} frame_info_t;

void renderPixels(SDL_Renderer *renderer, frame_info_t* frame_info);
void sketch(const char* filename, SDL_Renderer *renderer);
const char * const rgbstr(gd_color_t color);
void dumpGlobalColorTable(gd_color_t* table, size_t size);
void dumpPixel(frame_info_t *frame_info, uint16_t p);

void drawGif(SDL_Renderer *renderer) {
    // const char* filename = "samples/sample_1.gif";
    const char* filename = "samples/128px-green.gif";
    sketch(filename,renderer);
}

void renderPixels(SDL_Renderer *renderer, frame_info_t* frame_info) {
    SDL_Rect rect;
    rect.x = 50;
    rect.y = 50;
    rect.w = frame_info->zoom;
    rect.h = frame_info->zoom;

    SDL_RenderClear(renderer);
    for (uint8_t y=0; y<frame_info->height; y++) {
        for (uint8_t x=0; x<frame_info->width; x++) {
            rect.x = x * frame_info->zoom + 80;
            rect.y = y * frame_info->zoom + 80;
            uint16_t i = y * frame_info->width + x;
            gd_color_t c = frame_info->colors[frame_info->pixels[i]];
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
void sketch(const char* filename, SDL_Renderer *renderer) {
    gd_main_t main;
    gd_info_t info;    
    gd_graphic_control_extension_t gce;
    gd_image_descriptor_t imd;
    // allocated dynamically
    gd_color_t* gct = 0;
    gd_index_t* pixels = 0;
    frame_info_t frame_info;

    FILE* fp = fopen(filename, "rb");
    if (fp == NULL) {
        printf("error open file '%s'\n", filename);
        return;
    }
    // FILE* fp = fopen("samples/128px-Dancing.gif", "rb");
    main.fp = fp;
    // signature mismatch, should be opaque (no FILE)
    main.fread = (void*)fread;

    gd_init(&main);
    main.pixelOutputProgress = 777; // debug
    printf("gd init %d\n", main.err);
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
                printf("logical screen descripor: gct flag: %d global color table size: %d\n", info.globalColorTableFlag, info.globalColorTableSize);
                break;
            case GD_BLOCK_GLOBAL_COLOR_TABLE:
                gct = (gd_color_t*)calloc(info.globalColorTableSize, sizeof(gd_color_t));
                gd_read_global_color_table(&main, gct, info.globalColorTableSize);
                // check status
                break;
            case GD_BLOCK_GRAPHIC_CONTROL_EXTENSION:
                gd_read_graphic_control_extension(&main, &gce);
                break;
            case GD_BLOCK_IMAGE_DESCRIPTOR:
                gd_read_image_descriptor(&main, &imd);
                printf("image descriptor %d %d\n", imd.image_width, imd.image_height);
                break;
            case GD_BLOCK_IMAGE_DATA:
                printf("pixels: %zu\n", imd.image_size);
                // fixme memory leak
                pixels = (gd_index_t*)calloc(imd.image_size, sizeof(gd_index_t));
                const gd_index_t fill = 0x45;
                memset(pixels, fill, imd.image_size);

                // at this point we'd like to be able to re/allocate space for:
                // - global color table
                // - code buffer - optional, current stack allocation is ok
                // - string table entries and strings
                // also a helper function to estimate size for given image descriptor

                // gd_alloc_output(gd_info *info, void *mem, const size_t len)
                // gd_alloc_string_table_entries()
                // gd_alloc_string_table_strings()

                // maybe via gd_info, which already knows global color table size

                // consumer of gct: GD_BLOCK_GLOBAL_COLOR_TABLE, gd_read_global_color_table
                // consumer of output: GD_BLOCK_IMAGE_DATA, gd_read_image_data
                // conumser of entries: 


                gd_read_image_data(&main, pixels, imd.image_size);
                printf("pixels output: %zu\n", main.pixelOutputProgress);
                if (main.err != GD_X_OK) {
                    printf("read image data failed: err: %d\n", (int)main.err);
                    // todo clean up?
                    return;
                }
                
                frame_info.width = imd.image_width;
                frame_info.height = imd.image_height;
                frame_info.colors = gct;
                frame_info.pixels = pixels;
                frame_info.zoom = 4;
                renderPixels(renderer, &frame_info);
                break;
            case GD_BLOCK_TRAILER:
                printf("end of gif parsing\n");
                blockLimit = 0;
                printf("pixel[0] index: %d color: %s\n", frame_info.pixels[0], rgbstr(frame_info.colors[frame_info.pixels[0]]));
                // dumpPixel(&frame_info, 0);
                // dumpPixel(&frame_info, 1);
                // dumpPixel(&frame_info, 2);
                // dumpPixel(&frame_info, 3);
                // dumpGlobalColorTable(gct, info.globalColorTableSize);
                break;
            case GD_BLOCK_COMMENT_EXTENSION:
            case GD_BLOCK_PLAIN_TEXT_EXTENSION:
            case GD_BLOCK_APPLICATION_EXTENSION:
            case GD_BLOCK_LOGICAL_EOF:
                // ignored
                break;
        }
        if (main.err != GD_X_OK) {
            printf("aborted: err: %d\n", main.err);
            break;
        }
    }

    printf("bytes read from file: %d\n", ftell(fp));
    
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

const char * const rgbstr(gd_color_t color) {
    #define digits ((int)(3))
    static char buffer[(digits+1)*3];
    snprintf(buffer, sizeof(buffer), "%d,%d,%d", color.r, color.g, color.b);
    return buffer; // OK static
}
void dumpPixel(frame_info_t *frame_info, uint16_t p) {
    printf("pixel[%d] index: %d color: %s\n", p, frame_info->pixels[p], rgbstr(frame_info->colors[frame_info->pixels[p]]));
}
void dumpGlobalColorTable(gd_color_t* table, size_t size) {
    for (int i=0; i<size; i++) {
        printf("[%d] %s\n", i, rgbstr(table[i]));
    }
}