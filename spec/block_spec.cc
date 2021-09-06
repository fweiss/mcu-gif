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
    static gd_main_t main;
    static gd_info_t info;

    before("all", [&] {
        // todo seg fault when main not initilized
        main.read = f_read;
        gd_init(&main);
    });
    describe("header", [&] {
        before("all", [&] {
            FFILE(header_logical_screen_descriptor);
            gd_read_header(&main, &info);
        });
        it("next block type", [&] {
            expect(gd_next_block_type(&main)).to(eq(GD_BLOCK_GLOBAL_COLOR_TABLE));
        });
    });
    describe("logical screen descriptor", [&] {
        before("all", [&] {
            FFILE(header_logical_screen_descriptor);
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