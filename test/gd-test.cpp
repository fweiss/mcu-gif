#include "gd.h"
#include "gd_internal.h"

#include "gtest/gtest.h"
#include "fff.h"

DEFINE_FFF_GLOBALS;

static const uint8_t header1[13] = { 'G', 'I', 'F', '8', '9', 'a', 0x11, 0x00, 0x4, 0x00, 0xee, 0xff, 0x88 };

static const uint8_t *f_read_data = header1;
static long f_read_data_length = sizeof(header1);

static long f_read_pos;
long my_read(int fd, uint8_t *buf, long count) {
//    printf("read %d %p %ld: %lx\n", fd, buf, count, f_read_pos);
    long available = std::min(count, f_read_data_length - f_read_pos);
    memcpy(buf,  &f_read_data[f_read_pos], available);
    f_read_pos += available;
    return available;
}
#define USE_FILE_DATA(d) (f_read_data = d, f_read_data_length = sizeof(d), f_read_pos = 0)
#define USE_FAKE_FILE(d) { \
    f_read_data = d; \
    f_read_data_length = sizeof(d); \
    f_read_pos = 0; \
    f_read_fake.custom_fake = my_read; \
    }


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

// f_read fake function declaration (see fff)
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

    ASSERT_EQ(info.status, GD_OK);
    ASSERT_EQ(info.width, 17);
}

class InvalidHeader : public ::testing::Test {
protected:
    uint8_t bad_signature[13] = { 'c' };
    int fd = 1;
    void SetUp() override {
        USE_FAKE_FILE(bad_signature);
        gd_init(f_read);
        gd_begin(fd);
    }
    void TearDown() override {
        gd_end();
    }
};

TEST_F(InvalidHeader, status) {
    gd_info_t info;
    gd_info_get(&info);

    ASSERT_EQ(info.status, GD_BAD_SIGNATURE);
}

class ScreenDescriptor : public ::testing::Test {
protected:
    const uint8_t header1[13] = { 'G', 'I', 'F', '8', '9', 'a', 0x11, 0x00, 0x4, 0x00, 0xee, 0xff, 0x88 };
    int fd = 1;
    gd_info_t info;
    void SetUp() override {
        USE_FAKE_FILE(header1);
        gd_init(f_read);
        gd_begin(fd);
        gd_info_get(&info);
    }
    void TearDown() override {
        gd_end();
    }
};

TEST_F(ScreenDescriptor, width) {
    ASSERT_EQ(info.width, 17);
}

TEST_F( ScreenDescriptor, height) {
    ASSERT_EQ(info.height, 4);
}

TEST_F(ScreenDescriptor, global_color_table) {
    ASSERT_EQ(info.gct, true);
}

TEST_F(ScreenDescriptor, global_color_table_bits) {
    ASSERT_EQ(info.gctb, 7);
}

class RenderFrame : public ::testing::Test {
protected:
    void null_ipixerls();
};

TEST_F(RenderFrame, null_ipixels) {
    gd_frame_t frame;
    frame.pixels = 0;
    gd_render_frame(&frame);

    EXPECT_EQ(frame.status, GD_NULL_POINTER);
}

class RenderSquaresImage : public ::testing::Test {
protected:
    void SetUp() override {
        // http://www.matthewflickinger.com/lab/whatsinagif/bits_and_bytes.asp
        static const unsigned char sample1[] = {
                0x47, 0x49, 0x46, 0x38, 0x39, 0x61, 0x0A, 0x00, 0x0A, 0x00, 0x91, 0x00, 0x00,
                0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
                0x21, 0xF9, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x2C, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x0A, 0x00, 0x00, 0x02, 0x16, 0x8C, 0x2D, 0x99, 0x87, 0x2A, 0x1C, 0xDC, 0x33, 0xA0, 0x02, 0x75, 0xEC, 0x95, 0xFA, 0xA8, 0xDE, 0x60, 0x8C, 0x04, 0x91, 0x4C, 0x01, 0x00, 0x3B
        };
//        USE_FILE_DATA(sample1);
        USE_FAKE_FILE(sample1);

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
    uint16_t pixels[10*10]; // actually color table indices

    void ipixels();
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

    EXPECT_EQ(colortab.size, 4);
    EXPECT_EQ(colortab.colors[0].r, 0xff);
    EXPECT_EQ(colortab.colors[3].b, 0x00);
}
TEST_F(RenderSquaresImage, image_descriptor) {
    ASSERT_EQ(frame.width, 10);
    ASSERT_EQ(frame.height, 10);
    ASSERT_FALSE(frame.has_local_color_table);
}
TEST_F(RenderSquaresImage, ipixels) {
    EXPECT_EQ(pixels[0], 1);
    EXPECT_EQ(pixels[1], 1);
    EXPECT_EQ(pixels[5], 2);
    EXPECT_EQ(pixels[99], 1);
}
TEST_F(RenderSquaresImage, color_table_lookup) {
    uint32_t rgb = gd_lookup_rgb(1);
    EXPECT_EQ(rgb, 0x00ff0000);
}

class DecodeSubBlock : public ::testing::Test {
protected:
    void SetUp() override {

    }
    void TearDown() override {
    }
};

TEST_F(DecodeSubBlock, sub_block_size) {
    uint8_t sub_block[] = { 0x8C, 0x2D, 0x99, 0x87, 0x2A };

    uint16_t codes[1024] = { 0 };
    uint16_t code_count;
    gd_sub_block_decode_t decode;
    decode.minimum_code_size = 2;
    decode.sub_block_size = 2;
    decode.sub_block = sub_block;
    decode.codes = codes;
    decode.code_count = &code_count;
    gd_sub_block_decode(&decode);

    ASSERT_EQ(decode.status, GD_SUB_BLOCK_SIZE);

}

TEST_F(DecodeSubBlock, simple) {
    uint8_t sub_block[] = { 0x8C, 0x2D, 0x99, 0x87, 0x2A, 0x1C, 0xDC, 0x33, 0xA0, 0x02, 0x75, 0xEC, 0x95, 0xFA, 0xA8, 0xDE, 0x60, 0x8C, 0x04, 0x91, 0x4C, 0x01 };

    uint16_t codes[1024] = { 0 };
    uint16_t code_count;
    gd_sub_block_decode_t decode;
    decode.minimum_code_size = 2;
    decode.sub_block_size = 0x16;
    decode.sub_block = sub_block;
    decode.codes = codes;
    decode.code_count = &code_count;
    gd_sub_block_decode(&decode);

    ASSERT_EQ(decode.status, GD_OK);

    EXPECT_EQ(codes[0], 1);
    EXPECT_EQ(codes[1], 1);
    EXPECT_EQ(codes[2], 1);
    EXPECT_EQ(codes[3], 1);
    EXPECT_EQ(codes[4], 1);
    EXPECT_EQ(codes[5], 2);

    EXPECT_EQ(codes[10], 1);
    EXPECT_EQ(codes[35], 0);
    EXPECT_EQ(codes[50], 2);
    EXPECT_EQ(codes[95], 1);


}

class FileRead : public ::testing::Test {
protected:
    void SetUp() override {

    }
    void TearDown() override {
    }
    int fd = 1;
};

TEST_F(FileRead, eof_in_header) {
    const uint8_t shortheader[12] = { 'G', 'I', 'F', '8', '9', 'a', 0x11, 0x00, 0x4, 0x00, 0xee, 0xff };
    USE_FILE_DATA(shortheader);
    gd_init(f_read);
    gd_begin(fd);
    gd_info_t info;
    gd_info_get(&info);


    EXPECT_EQ(info.status, GD_READ_END);
}

class BlockType : public ::testing::Test {
protected:
    int fd = 1;
};

TEST_F(BlockType, image_descriptor) {
    const uint8_t data[14] = { 'G', 'I', 'F', '8', '9', 'a', 0x11, 0x00, 0x4, 0x00, 0xee, 0xff, 0x00, 0x00 };
    USE_FILE_DATA(data);
    gd_init(f_read);
    gd_begin(fd);
    gd_frame_t frame;
    gd_render_frame(&frame);
    gd_info_t info;
    gd_info_get(&info);

    EXPECT_EQ(info.status, GD_BLOCK_NOT_FOUND);

}
