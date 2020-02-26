#include "gd.h"
#include "gd_internal.h"

#include "gtest/gtest.h"
//#include "fff.h"

class DecodeLzw : public ::testing::Test {
protected:
    void SetUp() {
        lzw.code_size = 2;
        lzw.string_table_size_max = sizeof(string_table);
        lzw.string_table = string_table;
        lzw.characters = characters;
        lzw.characters_size = 0;
    }
    gd_lzw_t lzw;
    gd_string_t string_table[1024];
    uint16_t characters[1024];
    const uint8_t codes_1[36] = { 4, 1, 6, 6, 2, 9, 9, 7, 8, 10, 2, 12, 1, 14, 15, 6, 0, 21, 0, 10, 7, 22, 23, 18, 26, 7, 10, 29, 13, 24, 12, 18, 16, 36, 12, 5 };

    void test_initalize_minimal_code_table();
};

// todo code_size < 2 error

TEST_F(DecodeLzw, test_initalize_minimal_code_table) {
    gd_lzw_decode_next(&lzw, 0x04);

    EXPECT_EQ(lzw.string_table_size, 6); // include clear code and end code
    ASSERT_EQ(lzw.string_table[0].size, 1);
    ASSERT_EQ(lzw.string_table[1].size, 1);
    ASSERT_EQ(lzw.string_table[2].size, 1);
    EXPECT_EQ(lzw.string_table[3].characters[0], 3);
}

TEST_F(DecodeLzw, initialize_sample_code_table) {
    gd_lzw_decode_next(&lzw, 0x04);
    gd_lzw_decode_next(&lzw, 0x01);

    EXPECT_EQ(lzw.string_table_size, 6);
}

TEST_F(DecodeLzw, add_string_table) {
    gd_lzw_decode_next(&lzw, 0x04);
    gd_lzw_decode_next(&lzw, 0x03);
    gd_lzw_decode_next(&lzw, 0x06);

    EXPECT_EQ(lzw.string_table_size, 7);
}

TEST_F(DecodeLzw, characters_size) {
    for (int i=0; i<6; i++) {
        gd_lzw_decode_next(&lzw, codes_1[i]);
    }

    EXPECT_EQ(lzw.string_table_size, 4+2+6-2);
    // needs work
//    EXPECT_EQ(lzw.characters_size, 8);
//    EXPECT_EQ(lzw.characters[7], 2);
}

TEST_F(DecodeLzw, string_size_step) {
    gd_string_t *string;
    gd_lzw_decode_next(&lzw, 0x04);
    gd_lzw_decode_next(&lzw, 0x01);
    gd_lzw_decode_next(&lzw, 0x06);

    EXPECT_EQ(lzw.string_table_size, 7);
    string = &lzw.string_table[6];
    EXPECT_EQ(string->size, 2);

    gd_lzw_decode_next(&lzw, 0x06);
    string = &lzw.string_table[7];
    EXPECT_EQ(string->size, 3);

    gd_lzw_decode_next(&lzw, 2);
    string = &lzw.string_table[8];
    EXPECT_EQ(string->size, 3);
}

TEST_F(DecodeLzw, strings_size) {
    const uint16_t string_table_index = 6;
    for (int i=0; i<2+string_table_index; i++) {
        gd_lzw_decode_next(&lzw, codes_1[i]);
    }

    ASSERT_EQ(lzw.string_table_size, 6+string_table_index);
    gd_string_t *string = &lzw.string_table[6+string_table_index-1];
    EXPECT_EQ(string->size, 3);
}


TEST_F(DecodeLzw, string_table_21) {
    for (int i=0; i<2+21; i++) {
        gd_lzw_decode_next(&lzw, codes_1[i]);
    }

    ASSERT_EQ(lzw.string_table_size, 6+21);
    gd_string_t *string = &lzw.string_table[6+21-1];
//    EXPECT_EQ(string->size, 2);
}

TEST_F(DecodeLzw, four_one) {
    gd_lzw_decode_next(&lzw, 4); // init
    gd_lzw_decode_next(&lzw, 1); // freebee
    gd_lzw_decode_next(&lzw, 1); // 6 = [ 1 ]
    gd_lzw_decode_next(&lzw, 1); // 7 = [ 1, 1 ]

    EXPECT_EQ(lzw.string_table_size, 8);
    gd_string_t *string = &lzw.string_table[7];
    ASSERT_EQ(string->size, 2);
    EXPECT_EQ(string->characters[0], 1);
    EXPECT_EQ(string->characters[1], 1);
}
