#include "ccspec/core.h"
#include "ccspec/expectation.h"
#include "ccspec/matchers.h"

using ccspec::core::describe;
using ccspec::core::before;
using ccspec::core::it;
using ccspec::expect;
using ccspec::matchers::eq;
using ccspec::matchers::be;
using ccspec::matchers::be_truthy;

using std::vector;

extern "C" {
	#include "gd.h"
	#include "gd_internal.h"
}

#include "helpers/fake_file.h"
#include "helpers/pack.h"
#include "helpers/allocateMemory.h"

static const vector<uint8_t> header({ 'G', 'I', 'F', '8', '9', 'a' });
static const vector<uint8_t> logical_screen_descriptor({
    0x0A, 0x00, // width
    0x0A, 0x00, // height
    0x81, // color map info
    0x00, // background color index
    0x00 // aspect ratio
});
static const vector<uint8_t> logical_screen_descriptor_no_gct({
    0x0A, 0x00, // width
    0x0A, 0x00, // height
    0x01, // color map info
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
static const vector<uint8_t> image_descriptor({
    0x2C,       // image separator
    0x00, 0x00, // left
    0x00, 0x00, // top
    0x0A, 0x00, // width
    0x0A, 0x00, // height
    0x00,       // flags
});
static const vector<uint8_t> image_data({
    0x02, 0x16, 0x8C, 0x2D, 0x99, 0x87, 0x2A, 0x1C, 0xDC, 0x33, 0xA0, 0x02, 0x75, 0xEC, 0x95, 0xFA, 0xA8, 0xDE, 0x60, 0x8C, 0x04, 0x91, 0x4C, 0x01, 0x00,
});
static const vector<uint8_t> plain_text_extension({
    0x21, 0x01, // extension introducer, label
    0x0C, 0x00, 0x00, 0x00, 0x00, 0x64, 0x00, 0x64, 0x00, 0x14, 0x14, 0x01, 0x00,
    0x0B, 0x68, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x77, 0x6F, 0x72, 0x6C, 0x64,
    0x00,
});
static const vector<uint8_t> application_extension({
    0x21, 0xFF,
    0x0B, 0x4E, 0x45, 0x54, 0x53, 0x43, 0x41, 0x50, 0x45, 0x32, 0x2E, 0x30,
    0x03, 0x01, 0x05, 0x00,
    0x00
});
static const vector<uint8_t> comment_extension({
    0x21, 0xFE,
    0x09, 0x62, 0x6C, 0x75, 0x65, 0x62, 0x65, 0x72, 0x72, 0x79,
    0x00
});
static const vector<uint8_t> trailer({
    0x3b,
});

namespace simple {

// this needs to be globally visible for setup and debugging
gd_main_t main;

auto read_block_error_spec =
describe("read block error", [] {

    static gd_info_t info;

    before("all", [] {
        memset(&main, 0xee, sizeof(main));
        memset(&info, 0, sizeof(info));
        main.fread = ff_read;
        main.memory = allocate();
    });
    it("initial block", [] {
        gd_init(&main);
        expect((int)main.err).to(eq((int)GD_X_OK));
    });
    describe("header", [&] {
        before("all", [&] {
            FFILEV(header + trailer);
            gd_read_header(&main);
        });
        it("bytes", [&] {
            expect((int)ff_read_get_pos()).to(eq((int)6));
        });
    });
    describe("global color table", [&] {
        gd_color_t gct[100];
        it("eof", [&] {
            vector<uint8_t> short_global_color_table({ 0x00, 0x00 });
            FFILEV(logical_screen_descriptor + short_global_color_table);
            gd_read_logical_screen_descriptor(&main, &info);
            gd_read_global_color_table(&main, gct, info.globalColorTableSize);
            expect((int)main.err).to(be == (int)GD_ERR_EOF);
        });
    });
    describe("graphic control extension", [] {
        gd_graphic_control_extension_t gce;
        it("block prefix", [&] {
            // correct would be 21F9
            vector<uint8_t> file({ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 });
            FFILEV(file);
            gd_read_graphic_control_extension(&main, &gce);
            expect((int)main.err).to(be == (int)GD_ERR_BLOCK_PREFIX);
        });
    });

    describe("code table errors", [&] {
        before("each", [&] {
            main.err = GD_X_OK;
            main.memory = allocate();
        });
        describe("strings", [&] {
            before("each", [&] {
                main.memory.strings.sizeBytes = 7;
                FFILEV(image_data);
                static gd_index_t pixels[10];
                gd_read_image_data(&main, pixels, sizeof(pixels));
            });
            it("no space", [&] {
                expect((int)main.err).to(eq((int)GD_ERR_STRINGS_NO_SPACE));
            });
        });
        describe("entries", [&] {
            before("each", [&] {
                main.memory.entries.sizeBytes = 10;
                FFILEV(image_data);
                static gd_index_t pixels[10];
                // FIXME bytes or gd_index_t?
                gd_read_image_data(&main, pixels, sizeof(pixels));
            });
            it("no space", [&] {
                expect((int)main.err).to(eq((int)GD_ERR_ENTRIES_NO_SPACE));
            });
        });
    });

});

} // namespace
