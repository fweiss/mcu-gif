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

uint16_t gd_image_data_block_decode(gd_image_data_block_decode_t *decode, uint8_t *output) {
    int fd = -1;
    uint8_t buf[8];
    long count = decode->read(fd, buf, 8);
    long blockSize = buf[0];
    return blockSize == 1 ? 0 : 1;
}
