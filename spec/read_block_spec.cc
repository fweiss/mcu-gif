#include "ccspec/core.h"
#include "ccspec/expectation.h"
#include "ccspec/matchers.h"

using ccspec::core::describe;
using ccspec::core::before;
using ccspec::core::it;
using ccspec::expect;
using ccspec::matchers::eq;
using ccspec::matchers::be;

using std::vector;

extern "C" {
	#include "gd.h"
	#include "gd_internal.h"
}

#include "helpers/fake_file.h"

// a little DSL to concatenate vectors
// provides for constructing test files out of blocks
// doesn't have to be efficient
// vector<uint8_t> operator+(const vector<uint8_t> &a, const vector<uint8_t> &b) {
//     vector<uint8_t> r = a;
//     r.insert(r.end(), b.begin(), b.end());
//     return r;
// }

static const vector<uint8_t> header({ 'G', 'I', 'F', '8', '9', 'a' });
static const vector<uint8_t> logical_screen_descriptor({
    0x0A, 0x00, // width
    0x0A, 0x00, // height
    0x81, // color map info
    0x00, // background color index
    0x00 // aspect ratio
});
static const vector<uint8_t> global_color_table({
    0xff, 0xff, 0xff,
    0xff, 0x00, 0x00,
    0x00, 0x00, 0xff,
    0x00, 0x00, 0x00
});
static const vector<uint8_t> graphic_control_extension({
    0x21, 0xF9, // extension introducer, graphic control label
    0x04, // block size
    0x00,  0x00, 0x00, 0x00,
    0x00 // block terminator
});
static const vector<uint8_t> trailer({
    0x3b,
});

namespace simple {

    // um this is a lot like block_spec

auto read_block_spec =
describe("read block", [] {

    static gd_main_t main;
    static gd_info_t info;

    before("all", [] {
        memset(&main, 0, sizeof(main));
        memset(&info, 0, sizeof(info));
    });
    it("initial block", [] {
        gd_init(&main);
        gd_block_type_t type = gd_next_block_type(&main);
        expect(type).to(eq(GD_BLOCK_HEADER));
    });
    describe("header", [&] {
        before("all", [&] {
            FFILEV(header + trailer);
            main.read = f_read;
            gd_read_header2(&main);
        });
        it("bytes", [&] {
            expect((int)f_read_get_pos()).to(eq((int)6));
        });
    });
    describe("logical screen descriptor", [&] {
        static gd_info_t info;
        before("all", [&] {
            FFILEV(logical_screen_descriptor + trailer);
            main.read = f_read;
            gd_read_logical_screen_descriptor(&main, &info);
        });
        it("global color table size", [&] {
            expect((int)info.globalColorTableSize).to(eq((int)4));
        });
    });
    describe("global color table", [&] {
        static gd_color_t gct[4];
        before("all", [&] {
            FFILEV(global_color_table + trailer);
            main.read = f_read;
            gd_read_global_color_table(&main, gct);
        });
        it("read bytes", [&] {
            expect((int)f_read_get_pos()).to(eq((int)12));
        });
        it("next block type", [&] {
            // printf("next %d\n", gd_next_block_type(&main));
            expect((int)gd_next_block_type(&main)).to(eq((int)GD_BLOCK_GRAPHIC_CONTROL_EXTENSION));
        });
        it("gct[0].r", [&] {
            expect((int)gct[0].r).to(eq((int)0xff));
        });
        it("gct[1].r", [&] {
            expect((int)gct[1].r).to(eq((int)0xff));
        });
        it("gct[2].b", [&] {
            expect((int)gct[2].b).to(eq((int)0xff));
        });
    });
    describe("graphic control extension", [&] {
        gd_graphic_control_extension_t gce;
        before("all", [&] {
            vector<uint8_t> file = graphic_control_extension + trailer;
            FFILE(file.data());
            main.read = f_read;

            gd_read_graphic_control_extension(&main, &gce);
        });
        it("read bytes", [&] {
            expect((int)f_read_get_pos()).to(eq((int)8));
        });
        it("next block type", [&] {
            // printf("next %d\n", gd_next_block_type(&main));
            expect((int)gd_next_block_type(&main)).to(eq((int)GD_BLOCK_IMAGE_DESCRIPTOR));
        });
    });
    describe("image descriptor", [&] {
        static const vector<uint8_t> image_descriptor({
            0x2C, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x0A, 0x00, 0x00,
        });
        before("all", [&] {
            vector<uint8_t> file = image_descriptor + trailer;
            FFILEV(file);
            main.read = f_read;

            gd_read_image_descriptor(&main);
        });
        it("read bytes", [&] {
            expect((int)f_read_get_pos()).to(eq((int)10));
        });
    });
    describe("image data", [&] {
        static const vector<uint8_t> image_data({
            0x02, 0x16, 0x8C, 0x2D, 0x99, 0x87, 0x2A, 0x1C, 0xDC, 0x33, 0xA0, 0x02, 0x75, 0xEC, 0x95, 0xFA, 0xA8, 0xDE, 0x60, 0x8C, 0x04, 0x91, 0x4C, 0x01, 0x00,
        });
        static gd_index_t pixels[100];
        before("all", [&] {
            vector<uint8_t> file = image_data + trailer;
            FFILEV(file);
            main.read = f_read;

            gd_read_image_data(&main, pixels, 100);
        });
        it("read bytes", [&] {
            // last 0x00 not read?
            expect((int)f_read_get_pos()).to(eq((int)25-1));
        });
        it("next block type", [&] {
            expect((int)gd_next_block_type(&main)).to(eq((int)GD_BLOCK_TRAILER));
        });
        it("pixel[0]", [&] {
            expect((int)pixels[0]).to(eq((int)1));
        });
    });
});

} // namespace
