#pragma once

#include <stdint.h>
#include <stdio.h>
#include <vector>

using std::vector;

#if defined(_MSC_VER)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

extern void ff_open_memory(const uint8_t *data, long size);
extern void ff_open_memory(const vector<uint8_t> &data);
extern void ff_print_memory();
extern ssize_t ff_read(int fd, void *buf, size_t count);
extern size_t ff_read_get_pos();

#define FFILE(init) (ff_open_memory(init, sizeof(init)))
// memory errors when trying to pass expression directly to function
#define FFILEV(init) const vector<uint8_t> m = (init); ff_open_memory(m)

extern vector<uint8_t> operator+(const vector<uint8_t> &a, const vector<uint8_t> &b);
