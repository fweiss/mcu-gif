#include "gd.h"

void gd_open(gd_info_t *info) {
	info->width = 9;
}

void gd_decode(gd_decode_t *decode) {
	decode->colorTable[0] = 0xff0000ff;
	decode->pixels[0][0] = 0;
}
