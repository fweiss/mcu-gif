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

auto read_block_spec =
describe("read block", [] {

    static gd_main_t main;
    static gd_info_t info;

    before("all", [] {
        memset(&main, 0, sizeof(main));
        memset(&info, 0, sizeof(info));
        main.fread = ff_read;
    });
    it("initial block", [] {
        gd_init(&main);
        gd_block_type_t type = gd_next_block_type(&main);
        expect(type).to(eq(GD_BLOCK_HEADER));
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
    describe("logical screen descriptor", [&] {
        static gd_info_t info;
        before("all", [&] {
            FFILEV(logical_screen_descriptor + trailer);
            gd_read_logical_screen_descriptor(&main, &info);
        });
        it("bytes", [&] {
            expect((int)ff_read_get_pos()).to(eq((int)7));
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
            expect(info.globalColorTableSize).to(be == 4);
        });
    });
    describe("global color table", [&] {
        // could also try dynamic
        static gd_color_t gct[4];
        before("all", [&] {
            FFILEV(global_color_table + trailer);
            gd_read_global_color_table(&main, gct);
        });
        it("bytes", [&] {
            expect((int)ff_read_get_pos()).to(eq((int)12));
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
            FFILEV(graphic_control_extension + trailer);

            gd_read_graphic_control_extension(&main, &gce);
        });
        it("bytes", [&] {
            expect((int)ff_read_get_pos()).to(eq((int)8));
        });
    });
    describe("local color table", [] {

    });
    describe("image descriptor", [&] {
        static gd_image_descriptor_t imd;
        before("all", [&] {
            FFILEV(image_descriptor + trailer);

            gd_read_image_descriptor(&main, &imd);
        });
        it("bytes", [&] {
            expect((int)ff_read_get_pos()).to(eq((int)10));
        });
        it("image left", [] {
            expect(imd.image_width).to(be == 10);
        });
        it("image size", [] {
            expect(imd.image_size).to(be == 100);
        });
    });
    describe("image data", [&] {
        static gd_index_t pixels[100];
        before("all", [&] {
            FFILEV(image_data + trailer);
            gd_read_image_data(&main, pixels, 100);
        });
        it("bytes", [&] {
            // fixme last 0x00 not read?
            expect((int)ff_read_get_pos()).to(eq((int)25-1));
        });
        it("pixel[0]", [&] {
            expect((int)pixels[0]).to(eq((int)1));
        });
    });
    describe("plain text extension", [] {

    });
    describe("application extension", [] {

    });
    describe("comment extension", [] {

    });
    describe("trailer", [] {

    });
    describe("next block type", [&] {
        describe("after header", [&] {
            it("logical screen descriptor", [&] {
                FFILEV(header + logical_screen_descriptor);
                gd_read_header(&main);
                expect((int)gd_next_block_type(&main)).to(eq((int)GD_BLOCK_LOGICAL_SCREEN_DESCRIPTOR));
            });
        });
        describe("after logical screen descriptor", [&] {
            static gd_info_t info;
            it("global color table", [&] {
                FFILEV(logical_screen_descriptor + global_color_table);
                gd_read_logical_screen_descriptor(&main, &info);
                expect((int)gd_next_block_type(&main)).to(eq((int)GD_BLOCK_GLOBAL_COLOR_TABLE));
            });
            it("graphic control extension", [&] {
                vector<uint8_t> no_gct = logical_screen_descriptor;
                no_gct[4] &= ~0x80;
                FFILEV(no_gct + graphic_control_extension);
                gd_read_logical_screen_descriptor(&main, &info);
                expect((int)gd_next_block_type(&main)).to(eq((int)GD_BLOCK_GRAPHIC_CONTROL_EXTENSION));
            });
            // the following require file peek
            // it("image descriptor", [&] {
            //     FFILEV(logical_screen_descriptor_no_gct + image_descriptor);
            //     gd_read_logical_screen_descriptor(&main, &info);
            //     expect((int)gd_next_block_type(&main)).to(eq((int)GD_BLOCK_IMAGE_DESCRIPTOR));
            // });
            // it("plain text extension", [&] {
            //     FFILEV(logical_screen_descriptor_no_gct + plain_text_extension);
            //     gd_read_logical_screen_descriptor(&main, &info);
            //     expect((int)gd_next_block_type(&main)).to(eq((int)GD_BLOCK_PLAIN_TEXT_EXTENSION));
            // });
            // it("application extension", [&] {
            //     FFILEV(logical_screen_descriptor_no_gct + application_extension);
            //     gd_read_logical_screen_descriptor(&main, &info);
            //     expect((int)gd_next_block_type(&main)).to(eq((int)GD_BLOCK_APPLICATION_EXTENSION));
            // });
            // it("comment extension", [&] {
            //     FFILEV(logical_screen_descriptor_no_gct + comment_extension);
            //     gd_read_logical_screen_descriptor(&main, &info);
            //     expect((int)gd_next_block_type(&main)).to(eq((int)GD_BLOCK_COMMENT_EXTENSION));
            // });
        });
        describe("after global color table", [&] {
            static gd_color_t gct[4];
            it("graphic control extension", [&] {
                FFILEV(global_color_table + graphic_control_extension);
                gd_read_global_color_table(&main, gct);
                expect((int)gd_next_block_type(&main)).to(eq((int)GD_BLOCK_GRAPHIC_CONTROL_EXTENSION));
            });
            // image descriptor, plain text extension, application extension, comment extension
        });
        describe("after graphic control extension", [&] {
            static gd_graphic_control_extension_t gce;
            it("image description", [&] {
                FFILEV(graphic_control_extension + image_descriptor);
                gd_read_graphic_control_extension(&main, &gce);
                expect((int)gd_next_block_type(&main)).to(eq((int)GD_BLOCK_IMAGE_DESCRIPTOR));
            });
        });
        describe("after image descriptor", [&] {
            gd_image_descriptor_t imd;
            // local color table (based on flag)
            it("image data", [&] {
                FFILEV(image_descriptor + image_data);
                gd_read_image_descriptor(&main, &imd);
                expect((int)gd_next_block_type(&main)).to(eq((int)GD_BLOCK_IMAGE_DATA));
            });
        });
        describe("after image data", [&] {
            // trailer 3B
            // see also logical screen desciptor, global color table
            // graphic control extension 21F9
            // image descriptor 2C
            // plain text extensiom 2101
            // application extension 21FF
            // comment extension 21FE
            static gd_index_t pixels[100];
            it("trailer", [&] {
                FFILEV(image_data + trailer);
                gd_read_image_data(&main, pixels, 100);
                expect((int)gd_next_block_type(&main)).to(eq((int)GD_BLOCK_TRAILER));
            });
        });
    });
});

} // namespace
