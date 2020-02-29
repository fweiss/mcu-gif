#include "common-test.h"

DEFINE_FFF_GLOBALS;

// f_read fake function declaration (see fff)
DEFINE_FAKE_VALUE_FUNC(long, f_read, int, uint8_t*, long);

long f_read_pos;
long f_read_data_length;
const uint8_t *f_read_data;

long my_read(int fd, uint8_t *buf, long count) {
//    printf("read %d %p %ld: %lx\n", fd, buf, count, f_read_pos);
    long available = std::min(count, f_read_data_length - f_read_pos);
    memcpy(buf,  &f_read_data[f_read_pos], available);
    f_read_pos += available;
    return available;
}


