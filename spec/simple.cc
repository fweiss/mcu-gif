#include "ccspec/core.h"

#include "ccspec/expectation.h"
#include "ccspec/matchers.h"

using ccspec::core::describe;
using ccspec::core::before;
using ccspec::core::it;
using ccspec::expect;
using ccspec::matchers::eq;
using ccspec::matchers::be_truthy;

// fake file
static uint8_t *f_read_data = 0;
static long f_read_data_length = 0;
static long f_read_pos = 0;
static long f_read(int fd, uint8_t *buf, long count) {
//    printf("read %d %p %ld: %lx\n", fd, buf, count, f_read_pos);
    long available = std::min(count, f_read_data_length - f_read_pos);
    memcpy(buf,  &f_read_data[f_read_pos], available);
    f_read_pos += available;
    return available;
}
static void f_open_memory(uint8_t *data, long size) {
	f_read_data = data;
	f_read_data_length = size;
	f_read_pos = 0;
}

static uint8_t min_header[] = { 9, 9 };

static uint8_t header_logical_screen_descriptor[13] =
{
	'G', 'I', 'F', '8', '9', 'a',
	0x09, 0x00, // width
	0x09, 0x00, // height
	0x81, // color map info
	0x00, // background color index
	0x00 // aspect ratio
};

#define FFILE(init) (f_open_memory(init, sizeof(init)))

#include "gd.h"

namespace simple {

auto addition_spec =
describe("for 9x9 red-blue-white", [] {
	// seems like this should be inside next describes
	// but then it segfaults because before each isn't invoked for second it
	gd_info_t info;

	describe("info", [&info] {

		before("each", [&info] {
			FFILE(header_logical_screen_descriptor);
			info.read = f_read;
			gd_open(&info);
		});

		it("width", [&info] {
			expect(info.width).to(eq(9));
		});

		it("height", [&info] {
			expect(info.height).to(eq(9));
		});

		it("global color table flag", [&info] {
			expect(info.globalColorTableFlag).to(be_truthy);
		});

		it("global color table size", [&] {
			expect(info.globalColorTableSize).to(eq(4));
		});
	});

	describe("decodes indexed rgba", [] {
		gd_decode_t d;

		before("each", [&d] {
			gd_decode(&d);
		});

		it("pixel[0][0] red", [&d] {
			expect(d.colorTable[d.pixels[0][0]]).to(eq(0xff0000ff));
		});

	});
});

} // namespace simple
