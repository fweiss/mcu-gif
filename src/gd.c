#include "gd.h"
#include "gd_internal.h"

#include <stdbool.h>
#include <string.h>

static inline uint16_t gd_unpack_word(uint8_t bytes[2]) {
    return bytes[0] + (bytes[1] << 8);
}

void gd_code_size(gd_image_block_t *block, uint8_t codeSize) {
    block->codeBits = codeSize;
    const uint16_t one = 1;
    block->codeMask = (one << codeSize) - 1;
}

/**
 * Initialize the string table with 0..3 and reserve 5..6 for 
 * special control codes 4 and 5
 */
void gd_string_table_init(gd_string_table_t *table) {
    static gd_string_table_entry_t entries[64];
    static gd_index_t strings[512];

    table->entries = entries;
    table->length = 6;
    table->capacity = 64;
    table->strings = strings;
    table->strings_length = 4;
    table->strings_capacity = 512;

    for (int i=0; i<4; i++) {
        gd_string_table_entry_t *entry = &table->entries[i];
        entry->length = 1;
        entry->offset = i;
        table->strings[i] = i;
    }
}

/**
 * Lookup the string for the given code.
 * If the code is greater than the string table length, return 0,
 * which really should be an error.
 * @return the string
 * @error returned string with length 0 if the code is not in the string table
 */
gd_string_t gd_string_table_at(gd_string_table_t *table, uint16_t code) {
    static gd_string_t string;
    if (code < table->length) {
        gd_string_table_entry_t entry = table->entries[code];
        string.length = entry.length;
        string.value = &table->strings[entry.offset];
    } else {
        string.length = 0;
    }
    return string;
}

/**
 * Add the given string to the string table.
 * Assume that this is not a duplicate.
 * @return the the new code for this string
 * @error if the string table is full
 */
uint16_t gd_string_table_add(gd_string_table_t *table, gd_string_t *string) {
    const bool entries_has_space= table->length < table->capacity;
    const bool strings_has_space = table->strings_length + string->length < table->strings_capacity;
    if (entries_has_space && strings_has_space) {
        uint16_t code = table->length;
        gd_string_table_entry_t *entry = &table->entries[table->length++];
        entry->length = string->length;
        entry->offset = table->strings_length;
        memcpy((void*)&table->strings[table->strings_length], (void*)string->value, string->length * sizeof(gd_index_t));
        table->strings_length += string->length;

        table->status = GD_OK;
        return code;
    } else {
        table->status = GD_ERROR;
        return 0xFFFF;
    }
}
/**
 * Decompress an upacked code onto an index stream.
 * @param extract an unpacked code
 * 
 * Gist of the algorithm
 * C = found string
 * A = added string
 * P = prior string
 *
 * Found:
 *   Output C, Add P+C[0], Next prior: P+C[0]
 * Not found:
 *   Output: P+P[0], add P+P[0], Next prior: P+P[0]
 */
void gd_image_expand_code(gd_expand_codes_t *expand, uint16_t extract) {
    if (extract == 0x0004) {
        expand->compressStatus = 1;
        gd_string_table_init(&expand->string_table);
        expand->prior_string.length = 0;
        return;
    } else if (extract == 0x0005) {
        expand->compressStatus = 0;
        return;
    }
    if (expand->compressStatus == 0) {
        return;
    }

    static gd_index_t raw_string[64];
    gd_string_t new_string;
    new_string.value = raw_string;

    // lookup the code
    gd_string_t found_string = gd_string_table_at(&expand->string_table, extract);
    bool found = found_string.length != 0;

    // create new string from prior
    memcpy(new_string.value, expand->prior_string.value, expand->prior_string.length * sizeof(gd_index_t));
    new_string.value[expand->prior_string.length] = found ? found_string.value[0] : expand->prior_string.value[0];
    new_string.length = expand->prior_string.length + 1;

    // skip insert on initial code
    if (expand->prior_string.length > 0) {
        gd_string_table_add(&expand->string_table, &new_string);
        // check string_table.status
    }

    // propagate prior string
    expand->prior_string = found ? found_string : new_string;

    // output to index stream
    memcpy(&expand->output[expand->outputLength], expand->prior_string.value, expand->prior_string.length * sizeof(gd_index_t));
    expand->outputLength += expand->prior_string.length;

//    if (expand->string_table.length == 8) {
//        expand->codeSize = 4;
//    } else if (expand->string_table.length == 16) {
//        expand->codeSize = 5;
//    } else if (expand->string_table.length == 32) {
//        expand->codeSize = 6;
//    }
    // does code not fit in code size bits?
    if (expand->string_table.length >> expand->codeSize) {
        expand->codeSize++;
    }
}

// given an "image data subblock", unpack it to a "code stream"
// then decompress the "code stream" to an "index stream"
void gd_image_subblock_decode(gd_image_block_t *block, uint8_t *subblock, uint8_t count) {
    block->codeMask = 0x07; // move into block?
    block->codeBits = 3;
    uint16_t onDeck = 0;
    uint8_t onDeckBits = 0;
    uint16_t extract = 0;
    uint8_t topBits = 0;
    uint16_t top;

    for (int i=0; ; ) {
        // shift out of on deck
        while (onDeckBits >= block->codeBits) {
            extract = onDeck & block->codeMask;
            onDeck >>= block->codeBits;
            onDeckBits -= block->codeBits;

//            gd_image_expand_code(block, extract);
            gd_image_expand_code(&block->expand_codes, extract);
            if (block->expand_codes.codeSize != block->codeBits) {
                gd_code_size(block, block->expand_codes.codeSize);
            }
        }
        // shift into on deck
        if (topBits > 0) {
            uint8_t shiftBits = (topBits > 16 - onDeckBits) ? (16 - onDeckBits) : topBits;
            onDeck |= top << onDeckBits;
            onDeckBits += shiftBits;
            topBits -= shiftBits;
        }
        // shift into top
        if (topBits == 0 && onDeckBits < block->codeBits) { // lazy fetch
            if (i == count) {
                break;
            }
            top = subblock[i++];
            topBits = 8;
        }
    }
}

static void gd_expand_codes_init(gd_expand_codes_t *expand_codes, gd_index_t *output) {
    expand_codes->codeSize = 3;
    expand_codes->output = output;
    expand_codes->outputLength =0;
    expand_codes->compressStatus = 0;
}

/********************/
/***  PUBLIC API  ***/
/********************/

void gd_init(gd_main_t *main) {
    main->err = GD_X_OK;
    main->next_block_type = GD_BLOCK_HEADER;
}

gd_block_type_t gd_next_block_type(gd_main_t * main) {
    return main->next_block_type;
}

void gd_read_header(gd_main_t *main) {
    const size_t header_length = 6;
    uint8_t buf[header_length];
    GD_READ(buf, sizeof(buf));
    main->next_block_type = GD_BLOCK_LOGICAL_SCREEN_DESCRIPTOR;
}

void gd_read_logical_screen_descriptor(gd_main_t *main, gd_info_t *info) {
    const uint8_t GLOBAL_COLOR_TABLE_FLAG = 0x80;
    const uint8_t GLOBAL_COLOR_TABLE_SIZE = 0x07;
    uint8_t buf[7];
    GD_READ(buf, sizeof(buf)); // todo check count
    info->width = gd_unpack_word(&buf[0]);
    info->height = gd_unpack_word(&buf[8-6]);
    info->globalColorTableFlag = buf[10-6] & GLOBAL_COLOR_TABLE_FLAG;
    info->globalColorTableSize = 1 << ((buf[10-6] & GLOBAL_COLOR_TABLE_SIZE) + 1);
    // todo no dupe
    main->info.width = gd_unpack_word(&buf[0]);
    main->info.height = gd_unpack_word(&buf[8-6]);
    main->info.globalColorTableFlag = buf[10-6] & GLOBAL_COLOR_TABLE_FLAG;
    main->info.globalColorTableSize = 1 << ((buf[10-6] & GLOBAL_COLOR_TABLE_SIZE) + 1);

    // todo based on flag and peek
    if (main->info.globalColorTableFlag) {
        main->next_block_type = GD_BLOCK_GLOBAL_COLOR_TABLE;
    } else {
        main->next_block_type = GD_BLOCK_GRAPHIC_CONTROL_EXTENSION;
        // todo check for eof
        // cannot peek with open(), need fopen()
        // uint8_t block_id = main->peek(main->fd);
        // if (block_id == 0x21) {
        //     // 21,F9 = graphics control extansion
        //     // 21,01 = plain_text_extension
        //     // 21,FF = application extension
        //     // 21,fe = comment extension
        //     // maybe peek, then read 2 bytes, then parser skips those 2 bytes.
        //     main->next_block_type = GD_BLOCK_GRAPHIC_CONTROL_EXTENSION;
        // } else  if (block_id == 0x2c) {
        //     main->next_block_type = GD_BLOCK_IMAGE_DESCRIPTOR;
        // }
    }
}

// expect the client to get the proper count from info
// alternative is to remember it from the logical screen descriptor
void gd_read_global_color_table(gd_main_t *main, gd_color_t *color_table, size_t count) {
    // todo handle chunks
    // size_t x = main->info.globalColorTableSize * sizeof(gd_color_t);
    size_t want = count * sizeof(gd_color_t);
    size_t got = GD_READ((uint8_t*)color_table, want);
    if (got != want) {
        main->err = GD_ERR_EOF;
        return;
    }
    // todo peek
    main->next_block_type = GD_BLOCK_GRAPHIC_CONTROL_EXTENSION;
}

void gd_read_graphic_control_extension(gd_main_t *main, gd_graphic_control_extension_t *gce) {
    uint8_t buf[8];
    GD_READ(buf, sizeof(buf));
    if (buf[0] != 0x21 || buf[1] != 0xF9) {
        main->err = GD_ERR_BLOCK_PREFIX;
        return;
    }
    // todo peek
    main->next_block_type = GD_BLOCK_IMAGE_DESCRIPTOR;
}

void gd_read_image_descriptor(gd_main_t *main, gd_image_descriptor_t* imd) {
    uint8_t buf[10];
    GD_READ(buf, sizeof(buf));
    imd->image_left = gd_unpack_word(&buf[1+0]);
    imd->image_top = gd_unpack_word(&buf[1+2]);
    imd->image_width = gd_unpack_word(&buf[1+4]);
    imd->image_height = gd_unpack_word(&buf[1+6]);
    imd->image_size = imd->image_width * imd->image_height;
    main->next_block_type = GD_BLOCK_IMAGE_DATA;
}

void gd_image_block_read(gd_main_t *main, gd_image_block_t *image_block) {
    gd_expand_codes_init(&image_block->expand_codes, image_block->output);

    long count = GD_READ(&image_block->minumumCodeSize, 1);

    image_block->outputLength = 0;

    uint8_t subblockSize;
    GD_READ(&subblockSize, 1);
    static uint8_t subblock[255];
    count = GD_READ(subblock, subblockSize);

    gd_image_subblock_decode(image_block, subblock, subblockSize);

    image_block->outputLength = image_block->expand_codes.outputLength;
    main->pixelOutputProgress= image_block->outputLength;
}

void gd_read_image_data(gd_main_t *main, gd_index_t *output, size_t capacity) {
    gd_image_block_t image_block;
    image_block.output = output;
    image_block.outputLength = capacity;
    gd_image_block_read(main, &image_block);
    // to do peek
    main->next_block_type = GD_BLOCK_TRAILER;
}

void gd_read_trailer(gd_main_t *main) {
    uint8_t buf[1];
    GD_READ(buf, sizeof(buf));
    main->next_block_type = GD_BLOCK_LOGICAL_EOF;
}

