#include "gd.h"

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

uint16_t gd_image_sub_block_decode(gd_image_data_block_decode_t *decode, uint16_t *output) {
    const int fd = -1;
    uint8_t buf[8];
    long count = decode->read(fd, buf, 8);
    const long blockSize = buf[0];
    uint16_t outputLength = 0;
    if (blockSize == 2) {
        output[0] = 1;
        outputLength++;
    }
    return outputLength;
}

void gd_image_subblock_decode(gd_image_block_t *block, uint8_t *subblock, uint8_t count) {
    uint16_t codeMask = 0x07;
    uint8_t codeBits = 3;
    uint16_t onDeck = 0;
    uint8_t onDeckBits = 0;
    uint16_t extract = 0;
    uint8_t topBits = 0;
    uint16_t top;

    for (int i=0; ; ) {
        while (onDeckBits >= codeBits) {
            extract = onDeck & codeMask;
            onDeck >>= codeBits;
            onDeckBits -= codeBits;
            // hmm shortcut? unclear why this is relevant at this level of abstraction
            // possibly a test smell
            if (extract == 0x005) {
                onDeckBits = 0;
            }

            if (extract != 0x0004 && extract != 0x0005) {
                block->output[block->outputLength++] = extract;
            }
        }
        if (topBits > 0) {
            uint8_t shiftBits = (topBits > 16 - onDeckBits) ? (16 - onDeckBits) : topBits;
            onDeck |= top << onDeckBits;
            onDeckBits += shiftBits;
            topBits -= shiftBits;
        }
        if (topBits == 0 && onDeckBits < codeBits) { // lazy fetch
            if (i == count) {
                break;
            }
            top = subblock[i++];
            topBits = 8;
        }
    }
}

