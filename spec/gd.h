#include <stdint.h>

typedef struct {
	long (*read)(int, uint8_t*, long);
	uint16_t width;
	uint16_t height;
	uint8_t globalColorTableFlag;
	uint8_t globalColorTableSize;
} gd_info_t;

typedef struct {
	uint32_t *colorTable;
	uint8_t pixels[9][9];;
} gd_decode_t;

void gd_open(gd_info_t *info);
void gd_decode(gd_decode_t *decode);
