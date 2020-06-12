#include "ccspec/core.h"

#include "ccspec/expectation.h"
#include "ccspec/matchers.h"

using ccspec::core::describe;
using ccspec::core::before;
using ccspec::core::it;
using ccspec::expect;
using ccspec::matchers::eq;

//extern int mul(int, int);

typedef struct {
	uint32_t *colorTable;
	uint8_t pixels[9][9];;
} gd_decode_t;

void gd_decode(gd_decode_t *decode) {
	decode->colorTable[0] = 0xff0000ff;
	decode->pixels[0][0] = 0;
}

namespace simple {

auto addition_spec =
describe("for 9x9 red-blue-white test file", [] {

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
