#pragma once

#include "gd.h"

typedef uint16_t gd_code_t;

typedef struct {
    uint16_t length;
    gd_index_t *value;
} gd_string_t;

typedef struct {
    uint16_t length;
    uint16_t offset;
} gd_string_table_entry_t;

typedef enum {
    GD_OK,
    GD_ERROR,
} gd_status_t;

typedef struct {
    gd_status_t status;
    uint16_t capacity;
    uint16_t length;
    gd_string_table_entry_t *entries;
    uint16_t strings_capacity;
    uint16_t strings_length;
    gd_index_t *strings;
} gd_string_table_t;

typedef struct {
    uint8_t compressStatus;
    uint8_t codeSize;
    gd_string_table_t string_table;
    gd_string_t prior_string;
    gd_index_t *output;
    uint16_t outputLength;
} gd_expand_codes_t;

typedef struct {
    uint8_t minumumCodeSize;
    uint8_t codeBits;
    uint16_t codeMask;
    gd_index_t *output;
    uint16_t outputLength;
    uint8_t compressStatus;
    gd_expand_codes_t expand_codes;
} gd_image_block_t;

void gd_string_table_init(gd_string_table_t *string_table);
gd_string_t gd_string_table_at(gd_string_table_t *string_table, uint16_t);
uint16_t gd_string_table_add(gd_string_table_t *string_table, gd_string_t *string);
void gd_image_expand_code(gd_expand_codes_t *expand, uint16_t code);
void gd_image_subblock_decode(gd_image_block_t *block, uint8_t *subblock, uint8_t count);
void gd_image_block_read(gd_main_t *main, gd_image_block_t *block);
void gd_code_size(gd_image_block_t *block, uint8_t codeSize);
