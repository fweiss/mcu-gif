#include "gd.h"

#include "gtest/gtest.h"
#include "fff.h"

DEFINE_FFF_GLOBALS;

// http://www.matthewflickinger.com/lab/whatsinagif/bits_and_bytes.asp
static const unsigned char sample1[] = {
        0x47, 0x49, 0x46, 0x38, 0x39, 0x61, 0x0A, 0x00, 0x0A, 0x00, 0x91, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00, 0x21, 0xF9, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2C, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x0A, 0x00, 0x00, 0x02, 0x16, 0x8C, 0x2D, 0x99, 0x87, 0x2A, 0x1C, 0xDC, 0x33, 0xA0, 0x02, 0x75, 0xEC, 0x95, 0xFA, 0xA8, 0xDE, 0x60, 0x8C, 0x04, 0x91, 0x4C, 0x01, 0x00, 0x3B
};

static const uint8_t header1[13] = { 'G', 'I', 'F', '8', '9', 'a', 0x11, 0x00, 0x4, 0x00, 0xee, 0xff, 0x88 };

static const uint8_t *f_read_data = header1;
static long f_read_data_length = sizeof(header1);

static long f_read_pos;
long my_read(int fd, uint8_t *buf, long count) {
//    printf("read %d %p %ld: %lx\n", fd, buf, count, f_read_pos);
    memcpy(buf,  &f_read_data[f_read_pos], count);
    f_read_pos += count;
    return 13;
}
#define USE_FILE_DATA(d) (f_read_data = d, f_read_data_length = sizeof(d), f_read_pos = 0)

//TEST(render_frame, basic) {
//    uint32_t pixels[1][1];
//
//    gd_frame_t frame;
//    frame.request = 0;
//    frame.width = 1;
//    frame.height = 1;
//    frame.pixels = (uint32_t *)pixels;
//    gd_render_frame(&frame);
//
//    ASSERT_EQ(frame.status, 0);
//    ASSERT_EQ(pixels[0][0], 0x11223344);
//}

// fragmental reads
// error reads
// premature eof
// no eof

FAKE_VALUE_FUNC(long, f_read, int, uint8_t*, long);

TEST(begin, info) {
    USE_FILE_DATA(header1);
//    f_read_data = header1;
    f_read_fake.return_val = 1;
    f_read_fake.custom_fake = my_read;

    gd_init(f_read);
    int fd = 1;
    gd_begin(fd);
    gd_info_t info;

    gd_info_get(&info);

    ASSERT_EQ(info.status, 0);
    ASSERT_EQ(info.width, 17);
}

//TEST(begin, bad_signature) {
//    uint8_t bad_signature[13] = { 'c' };
//    USE_FILE_DATA(bad_signature);
//    f_read_fake.return_val = 1;
//    f_read_fake.custom_fake = my_read;
//
//    gd_init(f_read);
//    int fd = 1;
//    gd_begin(fd);
//    gd_info_t info;
//    gd_info_get(&info);
//
//    ASSERT_EQ(info.status, 1);
//}

class BadSignature : public ::testing::Test {
protected:
    void SetUp() override {
        USE_FILE_DATA(bad_signature);
        gd_init(f_read);
        gd_begin(fd);
    }
    uint8_t bad_signature[13] = { 'c' };
    int fd = 1;
    void TearDown() override {
        gd_end();
    }
};

TEST_F(BadSignature, status) {
    gd_info_t info;
    gd_info_get(&info);

    ASSERT_EQ(info.status, GD_BAD_SIGNATURE);
}

class Header : public ::testing::Test {
protected:
    void SetUp() override {
        USE_FILE_DATA(header1);
        gd_init(f_read);
        gd_begin(fd);
        gd_info_get(&info);
    }
    void TearDown() override {
        gd_end();
    }
    const uint8_t header1[13] = { 'G', 'I', 'F', '8', '9', 'a', 0x11, 0x00, 0x4, 0x00, 0xee, 0xff, 0x88 };
    int fd = 1;
    gd_info_t info;
};

TEST_F(Header, width) {
    ASSERT_EQ(info.width, 17);
}

TEST_F(Header, height) {
    ASSERT_EQ(info.height, 4);
}

TEST_F(Header, global_color_table) {
    ASSERT_EQ(info.gct, true);
}

TEST_F(Header, global_color_table_bits) {
    ASSERT_EQ(info.gctb, 3);
}

class RenderSquaresImage : public ::testing::Test {
protected:
    void SetUp() override {
        USE_FILE_DATA(sample1);
        gd_init(f_read);
        gd_begin(fd);
        frame.pixels = pixels;
        gd_render_frame(&frame);
    }
    void TearDown() override {
        gd_end();
    }
    int fd = 1;
    gd_frame_t frame;
    uint32_t pixels[10*10];
};

TEST_F(RenderSquaresImage, size) {
    gd_info_t info;
    gd_info_get(&info);
    ASSERT_EQ(info.width, 10);
    ASSERT_EQ(info.height, 10);
}

TEST_F(RenderSquaresImage, global_color_table) {
    gd_info_t info;
    gd_info_get(&info);
    ASSERT_EQ(info.gctb, 2);
    gd_colortab_t colortab;
    gd_global_colortab_get(&colortab);
    ASSERT_EQ(colortab.size, 4);
    ASSERT_EQ(colortab.colors[0].r, 0xff);
}
TEST_F(RenderSquaresImage, image_descriptor) {
    ASSERT_EQ(frame.width, 10);
    ASSERT_EQ(frame.height, 10);
    ASSERT_FALSE(frame.has_local_color_table);
}
TEST_F(RenderSquaresImage, color_table_indices) {
    ASSERT_EQ(pixels[0], 0x11223344);
}

class DecodeLzw : public ::testing::Test {
protected:
    void SetUp() override {

    }
    void TearDown() override {
    }
};

TEST_F(DecodeLzw, simple) {
    const uint8_t encoded[] = { 0x02, 0x16, 0x8C, 0x2D, 0x99, 0x87, 0x2A, 0x1C, 0xDC, 0x33, 0xA0, 0x02, 0x75, 0xEC, 0x95, 0xFA, 0xA8, 0xDE, 0x60, 0x8C, 0x04, 0x91, 0x4C, 0x01, 0x00 };
    uint8_t decoded[1024] = { 0 };
    const int cc = 12;
    gd_decode_lzw(cc, encoded, decoded);

    ASSERT_EQ(decoded[0], 0);
}



