#include "ccspec/core.h"

#include "ccspec/expectation.h"
#include "ccspec/matchers.h"

using ccspec::core::describe;
using ccspec::core::before;
using ccspec::core::it;
using ccspec::expect;
using ccspec::matchers::eq;

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
}

static uint8_t min_header[] = { 9, 9 };

#include "gd.h"

namespace simple {

auto addition_spec =
describe("for 9x9 red-blue-white test file", [] {

	describe("info", [] {
		gd_info_t info;

		before("each", [&info] {
			f_open_memory(min_header, sizeof(min_header));
			info.read = f_read;
			gd_open(&info);
		});

		it("width", [&info] {
			expect(info.width).to(eq(9));
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
