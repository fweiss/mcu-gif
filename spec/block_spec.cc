#include "spec_header.h"

namespace simple {

/**
 * All the block read commands return specific data to the client.
 * These are happy path tests. See
 * - file_read_spec for next_ block_type
 * - ?? for status errors
 */
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
    static const vector<uint8_t> graphic_control_extension({
        0x21, 0xF9, // extension introducer, graphic control label
        0x04, // block size
        0x00,  0x00, 0x00, 0x00,
        0x00 // block terminator
    });
    static const vector<uint8_t> image_descriptor({
        0x2C, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x0A, 0x00, 0x00,
    });
    static const vector<uint8_t> image_data({});

    static gd_main_t main;
    static gd_info_t info;

    before("all", [&] {
        // todo seg fault when main not initilized
        main.fread = ff_read;
        gd_init(&main);
        // fixme ccspec enum
        expect((int)gd_next_block_type(&main)).to(eq((int)GD_BLOCK_HEADER));
    });
    describe("image data", [] {
        static gd_color_t *gct;
        static gd_graphic_control_extension_t gce;
        static gd_index_t pixels[100];
        before("all", [] {
            FFILEV(header + logical_screen_descriptor + global_color_table
             + graphic_control_extension + image_descriptor + image_data);

            gd_read_header(&main);
            gd_read_logical_screen_descriptor(&main, &info);
            gct = (gd_color_t*)malloc(info.globalColorTableSize * sizeof(gct[0]));
            gd_read_global_color_table(&main, gct);
            gd_read_graphic_control_extension(&main, &gce);
            gd_read_image_descriptor(&main);
            gd_read_image_data(&main, pixels, sizeof(pixels));
        });
        after("all", [&] {
            if (gct) {
                free(gct);
                gct = 0;
            }
        });
        it("a pixel", [&] {
            expect(pixels[0]).to(eq(0x0000));
        });
        it("next block", [] {
            expect((int)gd_next_block_type(&main)).to(eq((int)GD_BLOCK_TRAILER));
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
});

} // namespace simple