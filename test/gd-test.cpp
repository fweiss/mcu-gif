#include "gd.h"

#include "gtest/gtest.h"

#include "fff.h"

DEFINE_FFF_GLOBALS;

TEST(basic, basic) {
    uint32_t pixels[1][1];

    gd_frame_t frame;
    frame.request = 0;
    frame.width = 1;
    frame.height = 1;
    frame.pixels = (uint32_t *)pixels;
    gd_render_frame(&frame);

    ASSERT_EQ(frame.status, 0);
    ASSERT_EQ(pixels[0][0], 0x11223344);
}
