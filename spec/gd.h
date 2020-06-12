#include <stdint.h>

typedef struct {
	uint32_t *colorTable;
	uint8_t pixels[9][9];;
} gd_decode_t;

void gd_decode(gd_decode_t *decode);
