#pragma once

#include <stdint.h>
#include <stdio.h>

#if defined(_MSC_VER)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

extern void f_open_memory(uint8_t *data, long size);
extern ssize_t f_read(int fd, void *buf, size_t count);

#define FFILE(init) (f_open_memory(init, sizeof(init)))
