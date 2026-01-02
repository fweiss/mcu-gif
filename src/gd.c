#include "gd.h"
#include "gd_internal.h"

#include <stdbool.h>
#include <string.h>

static inline uint16_t gd_unpack_word(uint8_t bytes[2]) {
    return bytes[0] + (bytes[1] << 8);
}

#define min(x,y) ((x)>(y)?(y):(x))

/**
 * Initialize the string table with 0..2^n-1
 * and reserve 2^n and 2^n+1 for 
 * special control codes
 */
void gd_string_table_init(gd_string_table_t *table, uint8_t minCodeSize) {
    const uint16_t initializedSize = 1 << minCodeSize;

    table->entries = (gd_string_table_entry_t*)table->memory.entries.memoryBytes;
    table->entries_length = initializedSize + 2;
    table->entries_capacity = table->memory.entries.sizeBytes / sizeof(gd_string_table_entry_t);

    table->strings = (gd_index_t*)table->memory.strings.memoryBytes;
    table->strings_length = initializedSize;
    table->strings_capacity = table->memory.strings.sizeBytes / sizeof(*table->strings);

    table->err = GD_OK;

    // note forced narrowing to gd_index_t, initializedSize is know to be <= 256
    for (uint16_t i=0; i<initializedSize; i++) {
        gd_string_table_entry_t *entry = &table->entries[i];
        entry->length = 1;
        entry->offset = i;
        // the index is uint16_t but the element id gd_index_t
        table->strings[i] = (gd_index_t)(i);
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
    if (code < table->entries_length) {
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
    const bool entries_has_space= table->entries_length < table->entries_capacity;
    const bool strings_has_space = table->strings_length + string->length < table->strings_capacity;
    if (entries_has_space && strings_has_space) {
        uint16_t code = table->entries_length;
        gd_string_table_entry_t *entry = &table->entries[table->entries_length++];
        entry->length = string->length;
        entry->offset = table->strings_length;
        memcpy((void*)&table->strings[table->strings_length], (void*)string->value, string->length * sizeof(gd_index_t));
        table->strings_length += string->length;

        table->err = GD_OK;
        return code;
    } else {
        // fixme s/staus/err/
        // unlikely both?
        table->err = entries_has_space ? GD_ERR_STRINGS_NO_SPACE : GD_ERR_ENTRIES_NO_SPACE;
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
gd_err_t gd_image_code_expand(gd_expand_codes_t *expand, uint16_t extract) {
    if (extract == expand->clearCode) {
        expand->compressStatus = 1;
        // table init same as clearcode, but codebits is 2 x
        gd_string_table_init(&expand->string_table, expand->minumumCodeSize );
        expand->prior_string.length = 0;
        return GD_OK;
    } else if (extract == expand->endCode) {
        expand->compressStatus = 0;
        return GD_OK;
    }
    if (expand->compressStatus == 0) {
        return GD_OK;
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
        if (expand->string_table.err != GD_OK) {
            return expand->string_table.err;
        }
    }

    // propagate prior string
    expand->prior_string = found ? found_string : new_string;

    // output to index stream
    if (expand->outputLength + expand->prior_string.length * sizeof(gd_index_t) > expand->outputCapacity) {
        return GD_ERR_OUTPUT_NO_SPACE;
    }
    memcpy(&expand->output[expand->outputLength], expand->prior_string.value, expand->prior_string.length * sizeof(gd_index_t));
    expand->outputLength += expand->prior_string.length;

    // does code not fit in code size bits?
    if (expand->string_table.entries_length >> expand->codeSize) {
        expand->codeSize++;
    }
    return GD_OK; // maybe code for increase code size?
}

// given an "image data subblock", unpack its "byte stream" to a "code stream"
// then decompress the "code stream" to an "index stream" via gd_image_code_expand()
// this can occur 0 or more times in an image block
// it can init the code table, 
// but the minumumCodeSize is determined by the parent image block
gd_err_t gd_image_subblock_unpack(gd_unpack_t *unpack, uint8_t *subblock, uint8_t count) {

    // fixme check for count 0
    for (uint8_t i=0; ; ) {
        // shift out of on deck
        while (unpack->onDeckBits >= unpack->codeBits) {
            unpack->extract = unpack->onDeck & unpack->codeMask;
            unpack->onDeck >>= unpack->codeBits;
            unpack->onDeckBits -= unpack->codeBits;

            gd_err_t err = gd_image_code_expand(&unpack->expandCodes, unpack->extract);
            if (unpack->expandCodes.string_table.err != GD_OK) {
                return unpack->expandCodes.string_table.err; // abort on error
            }
            if (err != GD_OK) {
                return err;
            }

            if (unpack->extract == unpack->expandCodes.clearCode) {
                unpack->codeBits = unpack->expandCodes.minumumCodeSize + 1;
                unpack->codeMask = (1 << unpack->codeBits) -1;
            }

            // increase code size (max is 12)
            // fixme: codeMask should be + 0
            if (unpack->expandCodes.string_table.entries_length == (unpack->codeMask + 1)) {
                unpack->codeBits += 1;
                unpack->codeMask = (1 << unpack->codeBits) - 1;
            }

            // printf("codeBits,extract,entries,out,block: %d %d %d %d %d\n", unpack->codeBits, unpack->extract, unpack->expandCodes.string_table.entries_length, unpack->expandCodes.outputLength, i);

        }
        // shift into on deck
        if (unpack->topBits > 0) {
            uint8_t onDeckVacant = 16 - unpack->onDeckBits;
            uint8_t shiftBits = min(onDeckVacant, unpack->topBits);
            unpack->onDeck |= unpack->top << unpack->onDeckBits;
            unpack->onDeckBits += shiftBits;
            unpack->top >>= shiftBits;
            unpack->topBits -= shiftBits;
        }
        // shift into top
        if (unpack->topBits == 0 && unpack->onDeckBits < unpack->codeBits) { // lazy fetch
            if (i == count) {
                break;
            }
            unpack->top = subblock[i++];
            unpack->topBits = 8;
        }
    }
    return GD_OK;
}

static void gd_expand_codes_init(gd_expand_codes_t *expand_codes, gd_index_t *output) {
    expand_codes->codeSize = 3;
    expand_codes->output = output;
    expand_codes->outputLength =0;
    expand_codes->compressStatus = 0;
}

// internal
void gd_image_block_read(gd_main_t *main, gd_image_block_t *image_block) {
    gd_expand_codes_init(&image_block->unpack.expandCodes, image_block->output);

    // fixme check length, error abort
    (void)GD_READ(&image_block->unpack.expandCodes.minumumCodeSize, 1);
    // todo test limits
    // following may be moved to code table init
    uint8_t codeBits = image_block->unpack.expandCodes.minumumCodeSize + 1;

    image_block->outputLength = 0;
    image_block->unpack.expandCodes.outputLength = 0;

    image_block->unpack.codeBits = codeBits;
    image_block->unpack.codeMask = (1 << codeBits) - 1;
    // initialize unpack
    image_block->unpack.onDeck = 0;        // holds the bits coming from the byte stream
    image_block->unpack.onDeckBits = 0;
    image_block->unpack.extract = 0;       // the fully assmbled code
    image_block->unpack.topBits = 0;
    image_block->unpack.expandCodes.codeSize = codeBits;
    image_block->unpack.expandCodes.clearCode = (1 << image_block->unpack.expandCodes.minumumCodeSize);
    image_block->unpack.expandCodes.endCode = image_block->unpack.expandCodes.clearCode + 1;
    image_block->unpack.expandCodes.compressStatus = 0;

    for (int wdt=0; wdt<5000; wdt++) {
        uint8_t subblockSize;
        GD_READ(&subblockSize, 1);
        if (subblockSize == 0) {
            return;
        }
        static uint8_t subblock[255]; // MAX_SUB_BLOCK_SIZE
        // fixme check count
        (void)GD_READ(subblock, subblockSize);

        gd_err_t err = gd_image_subblock_unpack(&image_block->unpack, subblock, subblockSize);
        if (err != GD_OK) {
            main->err = err;
            return;
        }
        image_block->outputLength = image_block->unpack.expandCodes.outputLength;
        main->pixelOutputProgress= image_block->unpack.expandCodes.outputLength;
        main->err = image_block->unpack.expandCodes.string_table.err;
    }
}

/**************************/
/***  START PUBLIC API  ***/
/**************************/

// API
void gd_init(gd_main_t *main) {
    main->err = GD_OK;
    main->next_block_type = GD_BLOCK_HEADER;
}

// API
gd_block_type_t gd_next_block_type(gd_main_t * main) {
    return main->next_block_type;
}

// API
void gd_read_header(gd_main_t *main) {
    #define header_length ((size_t)(6))
    uint8_t buf[header_length];
    GD_READ(buf, sizeof(buf));
    main->next_block_type = GD_BLOCK_LOGICAL_SCREEN_DESCRIPTOR;
}

// API
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
// API
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

// API
void gd_read_graphic_control_extension(gd_main_t *main, gd_graphic_control_extension_t *gce) {
    (void)gce;
    uint8_t buf[8];
    GD_READ(buf, sizeof(buf));
    if (buf[0] != 0x21 || buf[1] != 0xF9) {
        main->err = GD_ERR_BLOCK_PREFIX;
        return;
    }
    // todo peek
    main->next_block_type = GD_BLOCK_IMAGE_DESCRIPTOR;
}

// API
void gd_read_image_descriptor(gd_main_t *main, gd_image_descriptor_t* imd) {
    uint8_t buf[10];
    GD_READ(buf, sizeof(buf));
    imd->image_left = gd_unpack_word(&buf[1+0]);
    imd->image_top = gd_unpack_word(&buf[1+2]);
    imd->image_width = gd_unpack_word(&buf[1+4]);
    imd->image_height = gd_unpack_word(&buf[1+6]);
    imd->image_size = imd->image_width * imd->image_height;
    uint8_t flags = buf[9];
    main->next_block_type = (flags & 0x80)
        ? GD_BLOCK_LOCAL_COLOR_TABLE
        : GD_BLOCK_IMAGE_DATA;
}

// see gd_read_global_color_table
void gd_read_local_color_table(gd_main_t *main, gd_color_t *color_table, size_t count) {
    size_t want = count * sizeof(gd_color_t);
    size_t got = GD_READ((uint8_t*)color_table, want);
    if (got != want) {
        main->err = GD_ERR_EOF;
        return;
    }
    main->next_block_type = GD_BLOCK_IMAGE_DATA;
}

// API
void gd_read_image_data(gd_main_t *main, gd_index_t *output, size_t capacity) {
    gd_image_block_t image_block;
    image_block.output = output;
    image_block.outputLength = 0;

    image_block.unpack.expandCodes.output = output;
    image_block.unpack.expandCodes.outputLength = 0;
    image_block.unpack.expandCodes.outputCapacity = capacity;
    image_block.unpack.expandCodes.string_table.memory = main->memory;

    gd_image_block_read(main, &image_block);
    // main->err = image_block.unpack.expandCodes.string_table.err;
    // to do peek
    main->next_block_type = GD_BLOCK_TRAILER;
}

// API
void gd_read_trailer(gd_main_t *main) {
    uint8_t buf[1];
    GD_READ(buf, sizeof(buf));
    main->next_block_type = GD_BLOCK_LOGICAL_EOF;
}

/************************/
/***  END PUBLIC API  ***/
/************************/
