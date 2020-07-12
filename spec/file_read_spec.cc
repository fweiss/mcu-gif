#include "ccspec/core.h"
#include "ccspec/expectation.h"
#include "ccspec/matchers.h"

using ccspec::core::describe;
using ccspec::core::before;
using ccspec::core::it;
using ccspec::expect;
using ccspec::matchers::eq;
using ccspec::matchers::be;

#include "gd.h"
#include "gd_internal.h"

#include "helpers/fake_file.h"

namespace simple {

auto file_read_spec =
describe("file read", [] {
    static uint8_t header_logical_screen_descriptor[13] =
    {
        'G', 'I', 'F', '8', '9', 'a',
        0x0A, 0x00, // width
        0x0A, 0x00, // height
        0x81, // color map info
        0x00, // background color index
        0x00 // aspect ratio
    };
    it("initial block type", [] {
        gd_main_t main;
        gd_init(&main);
        gd_block_type_t type = gd_next_block_type(&main);
        expect(type).to(eq(GD_BLOCK_INITIAL));
    });
    it("global color table type", [] {
        gd_main_t main;
        gd_info_t info;
        FFILE(header_logical_screen_descriptor);
        main.read = f_read;

        gd_init(&main);
        gd_read_header(&main, &info);
        gd_block_type_t type = gd_next_block_type(&main);

        expect((int)type).to(eq((int)GD_BLOCK_GLOBAL_COLOR_TABLE));
    });
});

} // namespace
