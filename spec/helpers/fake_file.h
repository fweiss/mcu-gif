#pragma once

#include <stdint.h>
#include <stdio.h>
#include <vector>

extern void f_open_memory(uint8_t *data, long size);
extern ssize_t f_read(int fd, void *buf, size_t count);

#define FFILE(init) (f_open_memory(init, sizeof(init)))

#define FFILEV(vec) (f_open_memory(vec.data(), vec.size()))
