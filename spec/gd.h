#include <stdint.h>

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
    uint8_t minumumCodeSize;
} gd_image_data_block_decode_t;

typedef struct {
    uint16_t *output;
    uint16_t outputLength;
} gd_image_block_t;

void gd_open(gd_info_t *info);
void gd_decode(gd_decode_t *decode);
uint16_t gd_image_sub_block_decode(gd_image_data_block_decode_t *blockDecode, uint16_t *output);
void gd_image_subblock_decode(gd_image_block_t *block, uint8_t *subblock, uint8_t count);
