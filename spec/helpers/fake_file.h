#pragma once

#include <stdint.h>

extern void f_open_memory(uint8_t *data, long size);
extern long f_read(int fd, uint8_t *buf, long count);

#define FFILE(init) (f_open_memory(init, sizeof(init)))
