#include "gd.h"
#include "gd_internal.h"

#include <stdlib.h>
#include <string>

static inline uint16_t gd_unpack_word(uint8_t bytes[2]) {
    return bytes[0] + (bytes[1] << 8);
}

void gd_open(gd_info_t *info) {
    const uint8_t GLOBAL_COLOR_TABLE_FLAG = 0x80;
    const uint8_t GLOBAL_COLOR_TABLE_SIZE = 0x03;
    int fd = 0;
    uint8_t buf[13];
    int count = (*info->read)(fd, buf, sizeof(buf));
    info->width = gd_unpack_word(&buf[6]);
    info->height = gd_unpack_word(&buf[8]);
    info->globalColorTableFlag = buf[10] & GLOBAL_COLOR_TABLE_FLAG;
    info->globalColorTableSize = 1 << ((buf[10] & GLOBAL_COLOR_TABLE_SIZE) + 1);
}

void gd_decode(gd_decode_t *decode) {
    decode->colorTable[0] = 0xff0000ff;
    decode->imageData[0] = 0;
}

void gd_code_size(gd_image_block_t *block, uint8_t codeSize) {
    block->codeBits = codeSize;
    const uint16_t one = 1;
    block->codeMask = (one << codeSize) - 1;
}

// deprecated
static void gd_string_table_init(gd_expand_codes_t *expand) {
    gd_string_t *string = (gd_string_t*)malloc(sizeof(gd_string_t) + sizeof(uint16_t));
    string->data[0] = 0x0000;
    expand->codeTable = string;
}

void gd_string_table_init(gd_string_table_t *table) {
    static gd_string_table_entry_t entries[64];
    static uint16_t strings[512];

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

gd_string2_t gd_string_table_at(gd_string_table_t *table, uint16_t code) {
    static gd_string2_t string;
    if (code < table->length) {
        gd_string_table_entry_t entry = table->entries[code];
        string.length = entry.length;
        string.value = &table->strings[entry.offset];
    } else {
        string.length = 0;
    }
    return string;
}

uint16_t gd_string_table_add(gd_string_table_t *table, gd_string2_t *string) {
    const bool entries_has_space= table->length < table->capacity;
    const bool strings_has_space = table->strings_length + string->length < table->strings_capacity;
    if (entries_has_space && strings_has_space) {
        uint16_t code = table->length;
        gd_string_table_entry_t *entry = &table->entries[table->length++];
        entry->length = string->length;
        memcpy((void*)&table->strings[table->strings_length], (void*)string->value, string->length * 2);
        table->strings_length += string->length;
        return code;
    } else {
        return 0xFFFF;
    }
}

void gd_image_expand_code(gd_expand_codes_t *expand, uint16_t extract) {
    if (extract == 0x0004) {
        expand->compressStatus = 1;
        // init code table
        gd_string_table_init(expand);
        expand->codeTableSize = 6;
        return;
    } else if (extract == 0x0005) {
        expand->compressStatus = 0;
        return;
    }
    if (expand->compressStatus) {
        expand->output[expand->outputLength++] = extract;
    }

    if (extract == 6) {
        expand->codeSize = 4;
        expand->codeTableSize = 7;
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

static void gd_expand_codes_init(gd_expand_codes_t *expand_codes, uint16_t *output) {
    expand_codes->codeSize = 3;
    expand_codes->output = output;
    expand_codes->outputLength =0;
    expand_codes->compressStatus = 0;
}

void gd_image_block_read(gd_main_t *main, gd_image_block_t *image_block) {
    gd_expand_codes_init(&image_block->expand_codes, image_block->output);

    const int fd = 0;
    long count = main->read(fd, &image_block->minumumCodeSize, 1);

    image_block->outputLength = 0;

    uint8_t subblockSize;
    main->read(fd, &subblockSize, 1);
    static uint8_t subblock[255];
    count = main->read(fd, subblock, subblockSize);

    gd_image_subblock_decode(image_block, subblock, subblockSize);

    image_block->outputLength = image_block->expand_codes.outputLength;
}


