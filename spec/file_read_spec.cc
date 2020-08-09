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
vector<uint8_t> operator+(const vector<uint8_t> &a, const vector<uint8_t> &b) {
    vector<uint8_t> r = a;
    r.insert(r.end(), b.begin(), b.end());
    return r;
}

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

namespace simple {

auto file_read_spec =
describe("file next block type", [] {

    static gd_main_t main;
    static gd_info_t info;

    before("each", [] {
        memset(&main, 0, sizeof(main));
        memset(&info, 0, sizeof(info));
    });
    it("initial block", [] {
        gd_init(&main);
        gd_block_type_t type = gd_next_block_type(&main);
        expect(type).to(eq(GD_BLOCK_INITIAL));
    });
    it("global color table", [] {
        vector<uint8_t> file = header + logical_screen_descriptor;
        FFILE(file.data());

        main.read = f_read;

        gd_init(&main);
        gd_read_header(&main, &info);
        gd_block_type_t type = gd_next_block_type(&main);

        expect((int)type).to(eq((int)GD_BLOCK_GLOBAL_COLOR_TABLE));
    });
    it("graphic control extension", [] {
        vector<uint8_t> file = header + logical_screen_descriptor + global_color_table;
        FFILE(file.data());

        main.read = f_read;
        uint8_t gct[4 * 3];

        gd_init(&main);
        gd_read_header(&main, &info);
        gd_read_global_color_table(&main, gct);

        gd_block_type_t type = gd_next_block_type(&main);
        expect((int)type).to(eq((int)GD_BLOCK_GRAPHIC_CONTROL_EXTENSION));
    });
    describe("10x10 screen and 4 global colors", [] {
		static gd_main_t main;
		static gd_info_t info;
		static uint8_t gct[4 * 3];
		before("each", [] {
			vector<uint8_t> file = header + logical_screen_descriptor + global_color_table;
			FFILEV(file);
			main.read = f_read;

			gct[2] = 0xaa;

			gd_init(&main);
			gd_read_header2(&main, &info);
			gd_read_global_color_table(&main, gct);
		});
		it("has correct color[2]", [] {
			expect(gct[2]).to(eq(0xff));
		});
		it("has correct color[4]", [] {
			expect(gct[4]).to(eq(0x00));
		});

    });
});

} // namespace
