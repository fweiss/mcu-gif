#include "ccspec/core.h"

#include "ccspec/expectation.h"
#include "ccspec/matchers.h"

using ccspec::core::describe;
using ccspec::core::before;
using ccspec::core::it;
using ccspec::expect;
using ccspec::matchers::eq;
using ccspec::matchers::be_truthy;

#include "helpers/fake_file.h"

extern "C" {
	#include "gd.h"
}
#include "helpers/allocateMemory.h"

static uint8_t header_logical_screen_descriptor[13] =
{
    'G', 'I', 'F', '8', '9', 'a',
    0x0A, 0x00, // width
    0x0A, 0x00, // height
    0x81, // color map info
    0x00, // background color index
    0x00 // aspect ratio
};

namespace simple {

static gd_main_t main;
static gd_info_t info;

auto spec_10x10_red_blue_white =
describe("for 10x10 red-blue-white", [] {

    describe("info", [] {

        before("each", [&] {
            FFILE(header_logical_screen_descriptor);
            main.fread = &ff_read;
            gd_init(&main);
            gd_read_header(&main);
            gd_read_logical_screen_descriptor(&main, &info);
        });

        it("width", [&] {
            expect(info.width).to(eq(10));
        });

        it("height", [&] {
            expect(info.height).to(eq(10));
        });

        // error LNK2001: unresolved external symbol "class ccspec::matchers::BeTruthy const & const ccspec::matchers::be_truthy"
        it("global color table flag", [&] {
            expect(info.globalColorTableFlag).to(be_truthy);
        });

        it("global color table size", [&] {
            expect(info.globalColorTableSize).to(eq(4));
        });
    });

    describe("read image", [] {
        static uint8_t sample1[] = {
                0x47, 0x49, 0x46, 0x38, 0x39, 0x61, 0x0A, 0x00, 0x0A, 0x00, 0x91, 0x00, 0x00,
                0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
                0x21, 0xF9, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x2C, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x0A, 0x00, 0x00, 
                0x02, 0x16, 0x8C, 0x2D, 0x99, 0x87, 0x2A, 0x1C, 0xDC, 0x33, 0xA0, 0x02, 0x75, 0xEC, 0x95, 0xFA, 0xA8, 0xDE, 0x60, 0x8C, 0x04, 0x91, 0x4C, 0x01, 0x00, 
                0x3B
        };
        static gd_index_t output[100];
        before("each", [] {
            FFILE(sample1);
            // gd_main_t main;
            main.memory = allocate();
            main.fread = ff_read;
            gd_info_t info;
            gd_color_t gct[4];
            gd_graphic_control_extension_t gce;
            gd_image_descriptor_t imd;

            // we know the order of calls needed here
            // fixme fail on error
            gd_init(&main);
            gd_read_header(&main);
            gd_read_logical_screen_descriptor(&main, &info);
            gd_read_global_color_table(&main, gct, info.globalColorTableSize);
            gd_read_graphic_control_extension(&main, &gce);
            gd_read_image_descriptor(&main, &imd);
            gd_read_image_data(&main, output, sizeof(output));
            gd_read_trailer(&main);
        });
        it("pixel[5] 2", [] {
            expect(output[5]).to(eq(2));
        });
        it("pixel[10] 1", [] {
            expect(output[10]).to(eq(1));
        });
        it("pixel[33] 0", [] {
            expect(output[33]).to(eq(0));
        });
    });
});

} // namespace simple
