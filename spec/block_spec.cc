#include "spec_header.h"

namespace simple {

static uint8_t header_logical_screen_descriptor[13] = {
    'G', 'I', 'F', '8', '9', 'a',
    0x0A, 0x00, // width
    0x0A, 0x00, // height
    0x81, // color map info
    0x00, // background color index
    0x00 // aspect ratio
};

auto block_spec = describe("block read", [] {
    gd_main_t main;
    gd_info_t info;
    describe("header", [&] {
        before("each", [&] {
            // todo seg fault when main not initilized
            FFILE(header_logical_screen_descriptor);
            main.read = f_read;
            gd_init(&main);
            gd_read_header(&main, &info);
        });
        it("width", [&] {
            expect(info.width).to(eq(10));
        });
        it("global color table size", [&] {
            // fixme implicit uint8_t
            // expect(info.globalColorTableSize).to(be((uint8_t)4));
        });
        it("next block type", [&] {
            expect(gd_next_block_type(&main)).to(eq(GD_BLOCK_GLOBAL_COLOR_TABLE));
        });
    });
    describe("logical screen descriptor", [] {

    });
    describe("global color table", [] {

    });
    describe("graphic control extension", [] {

    });
    describe("image descriptor", [] {

    });
    describe("local color table", [] {

    });
    describe("image data", [] {

    });
    describe("plain text extension", [] {

    });
    describe("application extension", [] {

    });
    describe("comment extension", [] {

    });
    describe("trailer", [] {

    });
});

} // namespace simple