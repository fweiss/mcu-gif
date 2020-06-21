#pragma once

#include "gd.h"

typedef struct {
    uint8_t compressStatus;
    uint8_t codeSize;
    uint16_t *output;
    uint16_t outputLength;
} gd_expand_codes_t;

void gd_image_expand_code2(gd_expand_codes_t *expand, uint16_t code);
