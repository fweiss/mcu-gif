#include "ccspec/core.h"

#include "ccspec/expectation.h"
#include "ccspec/matchers.h"

using ccspec::core::describe;
using ccspec::core::before;
using ccspec::core::it;
using ccspec::expect;
using ccspec::matchers::eq;

//extern int mul(int, int);

void gd_decode() {

}

namespace simple {

auto addition_spec =
describe("for 9x9 red-blue-white test file", [] {

	describe("decodes indexed rgba", [] {
		uint8_t pixels[9][9];
		uint32_t colorTable[4];

		before("each", [&colorTable, &pixels] {
			colorTable[0] = 0xff0000ff;
			pixels[0][0] = 0;
		});

		it("pixel[0][0] red", [&pixels, &colorTable] {
			expect(colorTable[pixels[0][0]]).to(eq(0xff0000ff));
		});

	});
});

} // namespace simple
