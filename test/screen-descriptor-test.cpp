#include "common-test.h"

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
