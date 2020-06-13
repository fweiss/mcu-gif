#include "fake_file.h"

#include <algorithm>

// fake file
static uint8_t *f_read_data = 0;
static long f_read_data_length = 0;
static long f_read_pos = 0;

long f_read(int fd, uint8_t *buf, long count) {
//    printf("read %d %p %ld: %lx\n", fd, buf, count, f_read_pos);
    long available = std::min(count, f_read_data_length - f_read_pos);
    memcpy(buf,  &f_read_data[f_read_pos], available);
    f_read_pos += available;
    return available;
}
extern void f_open_memory(uint8_t *data, long size) {
    f_read_data = data;
    f_read_data_length = size;
    f_read_pos = 0;
}
