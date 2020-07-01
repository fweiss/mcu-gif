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

#include "gd.h"

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

auto spec_10x10_red_blue_white =
describe("for 10x10 red-blue-white", [] {
    // seems like this should be inside next describes
    // but then it segfaults because before each isn't invoked for second it

    describe("info", [] {
        static gd_info_t info;

        before("each", [&] {
            FFILE(header_logical_screen_descriptor);
            info.read = f_read;
            gd_open(&info);
        });

        it("width", [&] {
            expect(info.width).to(eq(10));
        });

        it("height", [&] {
            expect(info.height).to(eq(10));
        });

        it("global color table flag", [&] {
            expect(info.globalColorTableFlag).to(be_truthy);
        });

        it("global color table size", [&] {
            expect(info.globalColorTableSize).to(eq(4));
        });
    });

    describe("decodes indexed rgba", [] {
        gd_decode_t decode;
        uint8_t imageData[10 * 10];
        uint32_t colorTable[4];

        before("each", [&] {
            decode.imageData = imageData;
            decode.colorTable = colorTable;
            gd_decode(&decode);
        });

        it("pixel[0][0] red", [&] {
            expect(colorTable[imageData[0]]).to(eq(0xff0000ff));
        });

    });
    describe("read image", [] {
        it("a pixel", [] {
            // set file system read handler
            // read header, logical screen descriptor
            // read global color table
            // read graphic control extension
            // read image descriptor
            // read image
            // read trailer
        });
    });
});

} // namespace simple
