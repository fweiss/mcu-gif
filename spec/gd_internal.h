#pragma once

#include "gd.h"

typedef struct {
    uint8_t compressStatus;
    uint8_t codeSize;
    uint16_t *output;
    uint16_t outputLength;
} gd_expand_codes_t;

typedef struct {
    uint8_t minumumCodeSize;
    uint8_t codeBits;
    uint16_t codeMask;
    uint16_t *output;
    uint16_t outputLength;
    uint8_t compressStatus;
    gd_expand_codes_t expand_codes;
} gd_image_block_t;

void gd_image_expand_code2(gd_expand_codes_t *expand, uint16_t code);
void gd_image_subblock_decode(gd_image_block_t *block, uint8_t *subblock, uint8_t count);
void gd_image_expand_code(gd_image_block_t *block, uint16_t code);
void gd_image_block_read(gd_main_t *main, gd_image_block_t *block);
void gd_code_size(gd_image_block_t *block, uint8_t codeSize);
