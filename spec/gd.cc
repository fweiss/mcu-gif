#include "gd.h"

void gd_open(gd_info_t *info) {
	int fd = 0;
	uint8_t buf[8];
	int count = (*info->read)(fd, buf, sizeof(buf));
	info->width = buf[0];
}

void gd_decode(gd_decode_t *decode) {
	decode->colorTable[0] = 0xff0000ff;
	decode->pixels[0][0] = 0;
}
