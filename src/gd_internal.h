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
    // gd_status_t status;
    gd_err_t status; // fixme s/b err
    gd_memory_t memory;

    // deprecated move to gd_expand_codes_t
    // gd_code_t clearCode;
    // gd_code_t endCode;

    uint16_t entries_capacity;
    uint16_t entries_length;
    gd_string_table_entry_t *entries;
    uint16_t strings_capacity;
    uint16_t strings_length;
    gd_index_t *strings;
} gd_string_table_t;

// this data is used for decompressing codes from a LZW sub-block,
// expanding each unpacked code to zero or more indexes
// this may require re-initializing the code table (when clearCode is encountered)
// note that this resets the codeSize
// it has the state of the string table (aka code table)
// it has the state of the output index stream
typedef struct {
    uint8_t compressStatus;
    uint8_t codeSize;
    uint16_t clearCode;
    uint16_t endCode;
    gd_string_table_t string_table;
    gd_string_t prior_string;
    gd_index_t *output;
    uint16_t outputLength;

    // unpack state variables, needed for subblocks
    uint16_t onDeck;            // holds the bits coming from the byte stream
    uint8_t onDeckBits;
    uint16_t extract;           // the fully assmbled code
    uint8_t topBits;
    uint16_t top;               // the partially assmebled code 

} gd_expand_codes_t;

// the data used to process an image block
// and its constituent image sub-blocks
// see gd_read_image_data() initiator
// see gd_image_block_read()
// see gd_image_subblock_unpack() hmm
// one challenge of the nested structs is an attempt
// to limit the data a function can access
typedef struct gd_image_block_t {
    uint8_t minumumCodeSize;        // comes directly from the image block
    uint8_t codeBits;               // the current number of bits in the code
    uint16_t codeMask;              // mask for the bits in the code, derived from codeBits
    gd_index_t *output;             // the indexes expanded form th4 codes
    size_t outputLength;
    uint8_t compressStatus;
    gd_expand_codes_t expand_codes; // ?? narrowed for the 
} gd_image_block_t;

void gd_string_table_init(gd_string_table_t *string_table, uint8_t minCodeSize);
gd_string_t gd_string_table_at(gd_string_table_t *string_table, uint16_t);
uint16_t gd_string_table_add(gd_string_table_t *string_table, gd_string_t *string);
void gd_image_code_expand(gd_expand_codes_t *expand, uint16_t code);
void gd_image_subblock_unpack(gd_image_block_t *block, uint8_t *subblock, uint8_t count);
void gd_image_block_read(gd_main_t *main, gd_image_block_t *block);
void gd_code_size(gd_image_block_t *block, uint8_t codeSize);
