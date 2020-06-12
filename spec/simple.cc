#include "ccspec/core.h"

#include "ccspec/expectation.h"
#include "ccspec/matchers.h"

using ccspec::core::describe;
using ccspec::core::before;
using ccspec::core::it;
using ccspec::expect;
using ccspec::matchers::eq;

#include "gd.h"

namespace simple {

auto addition_spec =
describe("for 9x9 red-blue-white test file", [] {

	describe("info", [] {
		gd_info_t info;

		before("each", [&info] {
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
