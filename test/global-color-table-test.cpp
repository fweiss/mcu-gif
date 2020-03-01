#include "common-test.h"

const uint8_t color_table_spec = 0x80;

static const uint8_t eof_in_block[]  = { 'G', 'I', 'F', '8', '9', 'a', 0x11, 0x00, 0x4, 0x00, color_table_spec, 0xff, 0x88, 0x00 };


class GlobalColorTable : public ::testing::Test {
protected:
    int fd = 1;
};

TEST_F(GlobalColorTable, eof) {
    USE_FAKE_FILE(eof_in_block);
    gd_init(f_read);
    gd_begin(fd);
    gd_info_t info;
    gd_info_get(&info);
    ASSERT_EQ(info.gct, true);
    ASSERT_EQ(info.gctb, 1);
    ASSERT_EQ(info.status, GD_READ_END);
}
