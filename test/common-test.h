#pragma once

#include "gd.h"
#include "gd_internal.h"

#include "gtest/gtest.h"
#include "fff.h"

//DEFINE_FFF_GLOBALS;

static const uint8_t header1[13] = { 'G', 'I', 'F', '8', '9', 'a', 0x11, 0x00, 0x4, 0x00, 0xee, 0xff, 0x88 };

//const uint8_t *f_read_datax = header1;
//long f_read_data_lengthx = sizeof(header1);

extern long f_read_pos;
extern long my_read(int fd, uint8_t *buf, long count);
extern const uint8_t *f_read_data;
extern long f_read_data_length;

extern long my_read(int fd, uint8_t *buf, long count);

#define USE_FILE_DATA(d) (f_read_data = d, f_read_data_length = sizeof(d), f_read_pos = 0)
#define USE_FAKE_FILE(d) { \
    f_read_data = d; \
    f_read_data_length = sizeof(d); \
    f_read_pos = 0; \
    f_read_fake.custom_fake = my_read; \
    }

// f_read fake function declaration (see fff)
DECLARE_FAKE_VALUE_FUNC(long, f_read, int, uint8_t*, long);

