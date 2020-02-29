#include "gd.h"
#include "gd_internal.h"

#include <string.h>
#include <stdlib.h>

#define DEBUG 1
#if DEBUG
#include <stdio.h>
#endif

#define LE(a, b) ((a) | (b << 8))
#define BIT(bit, byte) ((1 << bit) & byte)

/* file system abstractions */

struct {
    long (*read)(int fd, uint8_t *buf, long count);
} gd_config;

#define READ(f, b, c) (gd_config.read(f, b, c))

//static uint8_t fd = 1;

/* decoder state */

static struct {
    int status;
    int fd;
    int width;
    int height;
    bool gct;
    uint8_t gctb;
    color_t *gctf;
} gd_state;

static void debug_string_table(gd_string_t *strings, uint16_t code_strings_size) {
    for (int i=0; i<code_strings_size; i++) {
        const gd_string_t *string = &strings[i];
        printf("string[%d] ", i);
        if (string->size && string->characters) {
            for (int j=0; j<string->size; j++) {
                printf("0x%0x, ", string->characters[j]);
            }
        }
        printf("\n");
    }
}

/* api functions */

void gd_init(read_func_t read) {
    gd_config.read = read;
}

/*
 * Begin the stream by parsing the header and logical screen descriptor.
 */
void gd_begin(int fd) {
    gd_state.status = GD_OK;
    gd_state.fd = fd;

    uint8_t header[13];
    long count = READ(fd, header, sizeof(header));
    if (count != 13) {
        gd_state.status = GD_READ_END;
        return;
    }
    // todo check count
    bool is_not_sig87a = strncmp((char*)header, "GIF87a", 6);
    bool is_not_sig89a = strncmp((char*)header, "GIF89a", 6);
    if (is_not_sig87a && is_not_sig89a) {
        gd_state.status = GD_BAD_SIGNATURE;
        return;
    }
    gd_state.width = LE(header[6], header[7]);
    gd_state.height = LE(header[8], header[9]);

    gd_state.gct = header[10] & 0x80;
    gd_state.gctb = (header[10] & 0x07) + 1;
    if (gd_state.gct) {
        const uint16_t count = (1 << gd_state.gctb);
        const uint16_t size = count * sizeof(color_t);
        gd_state.gctf = (color_t*)malloc(size);
        long read = READ(fd, (uint8_t*)gd_state.gctf, size);
        if (read == 0) {
            gd_state.status = 22;
        }
    }
    gd_state.status = GD_OK;
}

void gd_end() {
    if (gd_state.gctf) {
        free(gd_state.gctf);
    }
}

void gd_info_get(gd_info_t *info) {
    info->status = gd_state.status;
    info->width = gd_state.width;
    info->height = gd_state.height;
    info->gct = gd_state.gct;
    info->gctb = gd_state.gctb;
}

void gd_read_extension_block() {
    uint8_t block[7];
    long count = READ(gd_state.fd, block, sizeof(block));
}

void gd_read_image_descriptor(gd_frame_t *frame) {
    const uint8_t block_type_size = 1;
    uint8_t block[10 - block_type_size];
    long count = READ(gd_state.fd, block, sizeof(block));
    // fixme check count
    // skip left
    // skip top
    frame->width = LE(block[4], block[5]);
    frame->height = LE(block[6], block[7]);
    frame->has_local_color_table = BIT(7, block[8]);
    // skip local color table size
}

void gd_read_image_data(gd_frame_t *frame) {
    // todo read directly into gd_sub_clock_decode_t
    uint8_t minimum_code_size;
    long count = READ(gd_state.fd, &minimum_code_size, sizeof(minimum_code_size));

    uint8_t data_sub_block_size;
    count = READ(gd_state.fd, &data_sub_block_size, sizeof(data_sub_block_size));

    uint8_t *sub_block = (uint8_t*)malloc(data_sub_block_size);
    count = READ(gd_state.fd, sub_block, data_sub_block_size);

//    gd_decode_data_sub_block(frame, sub_block, data_sub_block_size);
    uint8_t codes[1024] = { 0 };
    uint16_t code_count;

    printf("input count %d\n", data_sub_block_size);
    gd_sub_block_decode_t decode;
    decode.minimum_code_size = 2;
    decode.sub_block_size = data_sub_block_size;
    decode.sub_block = sub_block;
    decode.codes = frame->pixels;
    // todo check size, null
    decode.code_count = &code_count;
    gd_sub_block_decode(&decode);

//    count = READ(gd_state.fd, &data_sub_block_size, sizeof(data_sub_block_size));

    free(sub_block);
}

uint8_t gd_read_block_type() {
    uint8_t block_type;
    long count = READ(gd_state.fd, &block_type, sizeof(block_type));
    return block_type;
}

void gd_find_block_image_descriptor() {
    uint8_t block_type;
    block_type = gd_read_block_type();  // 0x21
    gd_read_extension_block();

    block_type = gd_read_block_type(); // 0x2C
    if (block_type != 0x2C) {
        gd_state.status = GD_BLOCK_NOT_FOUND;
    }
}

void gd_render_frame(gd_frame_t *frame) {
    if (frame->pixels == NULL) {
        frame->status = GD_NULL_POINTER;
        return;
    }
    gd_find_block_image_descriptor();
    if (gd_state.status != GD_OK) {
        return;
    }
    gd_read_image_descriptor(frame);

    gd_read_image_data(frame);

//    block_type = gd_read_block_type();

    frame->status = 0;
}

void gd_string_table_init(gd_string_t *string_table, uint8_t string_table_size) {
    for (int i=0; i<string_table_size; i++) {
        gd_string_t *string = &string_table[i];
        string->characters = (uint16_t*)malloc(sizeof(uint16_t));
        string->characters[0] = i;
        string->size = 1;
    }
    // optional for debugging
    string_table[string_table_size].size = 0;
    string_table[string_table_size + 1].size = 0;
}

void gd_lzw_decode_next(gd_lzw_t *lzw, uint16_t code) {
    // fixme these are relative to code size
    const uint16_t clear_code = 4;
    const uint16_t end_of_information_code = 5;

    if (code == clear_code) {
        // todo reset current_code_size?
        const uint16_t string_table_size = 1 << lzw->code_size;
        gd_string_table_init(lzw->string_table, string_table_size);
        lzw->string_table_size = string_table_size + 2;
        lzw->previous_string = NULL;
    } else if (code == end_of_information_code) {
        lzw->status = GD_OK;
        return;
    } else {
        if (lzw->previous_string == NULL) {
            // optimized string_table[code]->characters[0]
            *lzw->characters++ = code;
            lzw->previous_string = &lzw->string_table[code];
            return;
        } else {
            // get prefix from prior string
            const bool found = code < lzw->string_table_size;
            gd_string_t *output_string = found ? &lzw->string_table[code] : lzw->previous_string;
            const uint16_t k_character = output_string->characters[0];

            // add to string table
            const uint16_t next_string_index = lzw->string_table_size++;
            gd_string_t *string = &lzw->string_table[next_string_index];
            const uint16_t previous_string_size = lzw->previous_string->size;
            string->size = previous_string_size + 1;
            string->characters = (uint16_t*)malloc(string->size * sizeof(uint16_t));
            for (int k=0; k<previous_string_size; k++) {
                string->characters[k] = lzw->previous_string->characters[k];
            }
            string->characters[previous_string_size] = k_character;

            // output the string
            for (int j=0; j<output_string->size; j++) {
                lzw->characters[lzw->characters_size++] = output_string->characters[j];
            }
            if (!found) {
                lzw->characters[lzw->characters_size++] = k_character;
            }

            lzw->previous_string = found ? output_string : string;
        }
    }

}

void gd_sub_block_decode(gd_sub_block_decode_t *decode) {
    uint16_t *start = decode->codes;

    uint8_t current_code_size = 1 << decode->minimum_code_size;

    gd_string_t code_table[100];
    uint8_t code_table_size = 6;

    uint16_t extract_mask = 0x0007;
    uint8_t extract_bits = 3;
    uint16_t extract;
    uint16_t ondeck;
    uint16_t advance;

    uint8_t ondeck_bits = 0;
    uint8_t advance_bits = 0;
    ondeck = 0;
    gd_string_t *previous_string = NULL;

    // some of these are reinitialized
    gd_lzw_t lzw;
    lzw.string_table = code_table;
    lzw.code_size = current_code_size;
    lzw.previous_string = NULL;
    lzw.characters = decode->codes;

    for (int i=0; i<100; i++) {
        if (advance_bits == 0) {
            if (decode->sub_block_size-- == 0) {
                decode->status = GD_SUB_BLOCK_SIZE;
                break;
            }
            advance = *decode->sub_block++;
            advance_bits = 8;
        }
        if (ondeck_bits < extract_bits) {
            // todo handle larger symbols to 12 bits
            ondeck |= advance << ondeck_bits;
            advance_bits = 0;
            ondeck_bits += 8;
        }
        extract = ondeck & extract_mask;
        ondeck >>= extract_bits;
        ondeck_bits -= extract_bits;

        printf("extract: %0x\n", extract);

        gd_lzw_decode_next(&lzw, extract);

        // increase code size
        if (lzw.string_table_size == 8) {
            extract_bits = 4;
            extract_mask = (1 << extract_bits) - 1;
        }
        if (lzw.string_table_size == 32) {
            extract_bits = 5;
            extract_mask = (1 << extract_bits) - 1;
        }
    }
    printf("output count %ld\n", decode->codes - start);
    for (int i=0; i<decode->codes - start; i++)
        printf("code[%d] %d\n", i, start[i]);
    debug_string_table(code_table, code_table_size);
}

void gd_global_colortab_get(gd_colortab_t *colortab) {
    colortab->size = 1 << (gd_state.gctb + 0); // parser added 1
    colortab->colors = gd_state.gctf;
}

uint32_t gd_lookup_rgb(uint16_t index) {
    color_t color = gd_state.gctf[index];
    return color.r << 16 | color.g << 8 | color.b;
}

