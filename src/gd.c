#include "gd.h"

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

static uint8_t fd = 1;

/* decoder state */

static struct {
    int status;
    int width;
    int height;
    bool gct;
    uint8_t gctb;
    color_t *gctf;
} gd_state;

typedef struct {
    uint8_t size;
    uint16_t *characters;
} gd_code_string_t;

/* api functions */

void gd_init(read_func_t read) {
    gd_config.read = read;
}

void gd_begin(int fd) {
    gd_state.status = GD_OK;

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
    info->status = gd_state.status;
    info->width = gd_state.width;
    info->height = gd_state.height;
    info->gct = gd_state.gct;
    info->gctb = gd_state.gctb;
}

void gd_read_extension_block() {
    uint8_t block[7];
    long count = READ(fd, block, sizeof(block));
}

void gd_read_image_descriptor(gd_frame_t *frame) {
    uint8_t block[10 -1];
    long count = READ(fd, block, sizeof(block));
    frame->width = LE(block[4], block[5]);
    frame->height = LE(block[6], block[7]);
    frame->has_local_color_table = BIT(7, block[8]);
}

void gd_decode_data_sub_block(gd_frame_t *frame, uint8_t *sub_block, uint8_t sub_block_size) {
    uint16_t first_code = sub_block[0] & 0x07;
    uint16_t second_code = (sub_block[0] >> 3) & 0x07;
    frame->pixels[0] = second_code;
}

void gd_read_image_data(gd_frame_t *frame) {
    uint8_t minimum_code_size;
    long count = READ(fd, &minimum_code_size, sizeof(minimum_code_size));
    uint8_t data_sub_block_size;
    count = READ(fd, &data_sub_block_size, sizeof(data_sub_block_size));
    uint8_t *sub_block = (uint8_t*)malloc(data_sub_block_size);
    count = READ(fd, sub_block, data_sub_block_size);

    gd_decode_data_sub_block(frame, sub_block, data_sub_block_size);

    count = READ(fd, &data_sub_block_size, sizeof(data_sub_block_size));

    free(sub_block);
}

uint8_t gd_read_block_type() {
    uint8_t block_type;
    long count = READ(fd, &block_type, sizeof(block_type));
    return block_type;
}

void gd_render_frame(gd_frame_t *frame) {
    uint8_t block_type;
    block_type = gd_read_block_type();  // 0x21
    printf("block type %x\n", block_type);
    gd_read_extension_block();

    block_type = gd_read_block_type(); // 0x2C
    printf("block type %x\n", block_type);
    gd_read_image_descriptor(frame);

    gd_read_image_data(frame);

    block_type = gd_read_block_type();

    frame->status = 0;
}

void gd_code_table_init(gd_code_string_t *table, uint8_t code_size) {
    for (int i=0; i<code_size; i++) {
        gd_code_string_t *string = &table[i];
        string->characters = (uint16_t*)malloc(sizeof(uint16_t));
        string->characters[0] = i;
        string->size = 1;
    }
}

void gd_sub_block_decode(gd_sub_block_decode_t *decode) {
    uint8_t current_code_size = 1 << decode->minimum_code_size;
    const uint16_t clear_code = 4;
    const uint16_t end_of_information_code = 5;

    gd_code_string_t code_table[100];
    uint8_t code_table_size = 6;

    uint16_t extract_mask = 0x0007;
    uint8_t extract_bits = 3;
    uint16_t extract;
    uint16_t ondeck;
    uint16_t advance;

    uint8_t ondeck_bits = 0;
    uint8_t advance_bits = 0;
    ondeck = 0;
    gd_code_string_t *previous_string = NULL;
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

        if (extract == clear_code) {
            // todo reset current_code_size?
            gd_code_table_init(code_table, current_code_size);
            code_table_size = 6;
        } else if (extract == end_of_information_code) {
            decode->status = GD_OK;
            break;
        } else if (previous_string == NULL) {
            *decode->codes++ = extract;
            previous_string = &code_table[extract];
        } else {
            // get prefix from prior string
            const bool found = extract < code_table_size;
            const gd_code_string_t *output_string = found ? &code_table[extract] : previous_string;
            const uint16_t k_code = output_string->characters[0];

            // add to string table
            const uint16_t next_code = code_table_size++;

            gd_code_string_t *string = &code_table[next_code];
            const uint16_t previous_string_size = previous_string->size;
            string->size = previous_string_size + 1;
            string->characters = (uint16_t*)malloc(string->size * sizeof(uint16_t));
            for (int k=0; k<previous_string_size; k++) {
                string->characters[k] = previous_string->characters[k];
            }
            string->characters[previous_string_size] = k_code;

            // output the string
            for (int j=0; j<output_string->size; j++) {
                *decode->codes++ = output_string->characters[j];
            }
            if (!found) {
                *decode->codes++ = k_code;
            }

            previous_string = string;

            // increase code size
            if (code_table_size == 8) {
                extract_bits = 4;
                extract_mask = (1 << extract_bits) - 1;
            }
            if (code_table_size == 32) {
                extract_bits = 5;
                extract_mask = (1 << extract_bits) - 1;
            }
        }
    }
}

void gd_global_colortab_get(gd_colortab_t *colortab) {
    colortab->size = 1 << (gd_state.gctb + 0); // parser added 1
    colortab->colors = gd_state.gctf;
}

uint32_t gd_lookup_rgb(uint16_t index) {
    color_t color = gd_state.gctf[index];
    return color.r << 16 | color.g << 8 | color.b;
}

