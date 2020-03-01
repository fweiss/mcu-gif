#include "common-test.h"

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
