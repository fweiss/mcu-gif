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

void gd_image_block_read(gd_main_t *main, gd_image_block_t *image_block) {
    gd_expand_codes_init(&image_block->expand_codes, image_block->output);

    long count = main->read(main->fd, &image_block->minumumCodeSize, 1);

    image_block->outputLength = 0;

    uint8_t subblockSize;
    main->read(main->fd, &subblockSize, 1);
    static uint8_t subblock[255];
    count = main->read(main->fd, subblock, subblockSize);

    gd_image_subblock_decode(image_block, subblock, subblockSize);

    image_block->outputLength = image_block->expand_codes.outputLength;
}

void gd_init(gd_main_t *main) {
    main->next_block_type = GD_BLOCK_INITIAL;
}

void gd_read_header(gd_main_t *main, gd_info_t *info) {
    const size_t header_length = 6;
    const size_t logical_screen_descriptor_length = 7;
    const size_t global_color_table_length = 12;
    const size_t graphic_control_extension_length = 8;
    const size_t image_descriptor_length = 10;
    // MSVC C2057, C99 does not allow this kind of constant expression
    // uint8_t buf[header_length + logical_screen_descriptor_length + global_color_table_length + graphic_control_extension_length + image_descriptor_length];
    uint8_t buf[6+7+12+8+10];

    main->read(main->fd, buf, header_length);
//    main->read(main->fd, buf, logical_screen_descriptor_length);
    gd_read_logical_screen_descriptor(main, info);
    main->read(main->fd, buf, global_color_table_length);
    main->read(main->fd, buf, graphic_control_extension_length);
    main->read(main->fd, buf, image_descriptor_length);
    main->next_block_type = GD_BLOCK_GLOBAL_COLOR_TABLE;
}

void gd_read_header2(gd_main_t *main, gd_info_t *info) {
    const size_t header_length = 6;
    const size_t logical_screen_descriptor_length = 7;
    const size_t global_color_table_length = 12;
    const size_t graphic_control_extension_length = 8;
    const size_t image_descriptor_length = 10;
    uint8_t buf[header_length + logical_screen_descriptor_length + global_color_table_length + graphic_control_extension_length + image_descriptor_length];;

    main->read(main->fd, buf, header_length);
    gd_read_logical_screen_descriptor(main, info);
}
void gd_read_header3(gd_main_t *main) {
    const size_t header_length = 6;
    const size_t logical_screen_descriptor_length = 7;
    const size_t global_color_table_length = 12;
    const size_t graphic_control_extension_length = 8;
    const size_t image_descriptor_length = 10;
    uint8_t buf[header_length];;

    main->read(main->fd, buf, header_length);

    // TODO validation
    main->next_block_type = GD_BLOCK_LOGICAL_SCREEN_DESCRIPTOR;
}
void gd_read_logical_screen_descriptor(gd_main_t *main, gd_info_t *info) {
    const uint8_t GLOBAL_COLOR_TABLE_FLAG = 0x80;
    const uint8_t GLOBAL_COLOR_TABLE_SIZE = 0x03;
    uint8_t buf[7];
    int count = main->read(main->fd, buf, sizeof(buf));
    info->width = gd_unpack_word(&buf[6-6]);
    info->height = gd_unpack_word(&buf[8-6]);
    info->globalColorTableFlag = buf[10-6] & GLOBAL_COLOR_TABLE_FLAG;
    info->globalColorTableSize = 1 << ((buf[10-6] & GLOBAL_COLOR_TABLE_SIZE) + 1);
    main->next_block_type = GD_BLOCK_GLOBAL_COLOR_TABLE;
}

void gd_read_global_color_table(gd_main_t *main, uint8_t *color_table) {
	const size_t color_table_size = 4 * 3;
	main->read(main->fd, color_table, color_table_size);
    main->next_block_type = GD_BLOCK_GRAPHIC_CONTROL_EXTENSION;
}

void gd_read_image(gd_main_t *main, gd_index_t *output, size_t capacity) {
    gd_image_block_t image_block;
    image_block.output = output;
    image_block.outputLength = capacity;
    gd_image_block_read(main, &image_block);
}

gd_block_type_t gd_next_block_type(gd_main_t * main) {
    return main->next_block_type;
}


