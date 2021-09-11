#include "spec_header.h"

namespace simple {

auto block_spec = describe("block read", [] {
    // NB these have to be static
    // todo common const data fixture
    static const vector<uint8_t> header({
        'G', 'I', 'F', '8', '9', 'a',
    });
    static const vector<uint8_t> logical_screen_descriptor({
        0x0A, 0x00, // width
        0x0A, 0x00, // height
        0x81, // color map info
        0x00, // background color index
        0x00 // aspect ratio
    });
    static const vector<uint8_t> global_color_table({
        0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00
    });
    static gd_main_t main;
    static gd_info_t info;

    before("all", [&] {
        // todo seg fault when main not initilized
        main.read = f_read;
        gd_init(&main);
    });
    describe("header", [&] {
        before("all", [&] {
            FFILEV(header);

            gd_read_header(&main, &info);
        });
        it("next block type", [&] {
            expect(gd_next_block_type(&main)).to(eq(GD_BLOCK_GLOBAL_COLOR_TABLE));
        });
    });
    describe("logical screen descriptor", [&] {
        before("all", [&] {
            FFILEV(header + logical_screen_descriptor);

            gd_read_header2(&main);
            gd_read_logical_screen_descriptor(&main, &info);
        });
        it("width", [&] {
            expect(info.width).to(eq(10));
        });
        it("global color table size", [&] {
            // fixme implicit uint8_t
            // expect(info.globalColorTableSize).to(be(4));
            expect(info.globalColorTableSize).to(be == 4);
        });
        it("next block type", [&] {
            expect(gd_next_block_type(&main)).to(eq(GD_BLOCK_GLOBAL_COLOR_TABLE));
        });
    });
    describe("global color table", [&] {
        static uint8_t gct[12];
        before("all", [&] {
            FFILEV(header + logical_screen_descriptor + global_color_table);

            gd_read_header2(&main);
            gd_read_logical_screen_descriptor(&main, &info);
            // gct = (uint8_t*)malloc(info.globalColorTableSize * 3);
            gd_read_global_color_table(&main, gct);
        });
        after("all", [&] {
            // if (gct) {
            //     free(gct);
            //     gct = 0;
            // }
        });
        it("has color", [&] {
            expect((int)gct[0]).to(eq((int)0xff));
        });
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