#include "fake_file.h"

#include <algorithm>

static const uint8_t *f_read_data = 0;
static size_t f_read_data_length = 0;
static size_t f_read_pos = 0;

ssize_t f_read(int fd, void *buf, size_t count) {
//    printf("read %d %p %ld: %lx\n", fd, buf, count, f_read_pos);
    size_t available = std::min(count, f_read_data_length - f_read_pos);
    memcpy(buf, &f_read_data[f_read_pos], available);
    f_read_pos += available;
    return available;
}
extern void f_open_memory(const uint8_t *data, long size) {
    f_read_data = data;
    f_read_data_length = size;
    f_read_pos = 0;
}

extern void f_open_memory(const vector<uint8_t> &data) {
    // f_open_memory(data.data(), data.size());
    f_read_data = data.data();
    f_read_data_length = data.size();
    f_read_pos = 0;
}
