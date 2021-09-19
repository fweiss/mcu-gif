#include "fake_file.h"

#include <algorithm>
#include <iostream>

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

extern size_t f_read_get_pos() {
    return f_read_pos;
}

extern void f_print_memory() {
    std::cout << "=== hello " << std::hex;
    // for (auto uint8_t& c : x)
    for (int i=0; i < f_read_data_length; i++)
        std::cout << "'" << (uint8_t)f_read_data[i]+0 << "' ";
    std::cout << "\n";
}

// a little DSL to concatenate vectors
// provides for constructing test files out of blocks
// doesn't have to be efficient
std::vector<uint8_t> operator+(const std::vector<uint8_t>& lhs, const std::vector<uint8_t>& rhs)
{
    if (lhs.empty()) return rhs;
    if (rhs.empty()) return lhs;
    std::vector<uint8_t> result {};
    // printf("+ %d\n", lhs[0]);
    result.reserve(lhs.size() + rhs.size());
    result.insert(result.cend(), lhs.cbegin(), lhs.cend());
    result.insert(result.cend(), rhs.cbegin(), rhs.cend());
    return result;
}
