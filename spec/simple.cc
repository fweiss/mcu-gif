#include "ccspec/core.h"

#include "ccspec/expectation.h"
#include "ccspec/matchers.h"

using ccspec::core::describe;
using ccspec::core::it;
using ccspec::expect;
using ccspec::matchers::eq;

//extern int mul(int, int);

namespace simple {

auto addition_spec =
describe("Addition", [] {
	it("1 + 1 = 2", [] {
		expect(1 + 1).to(eq(2));
	});

//	it("2 * 3 = 6", [] {
//		expect(mul(2, 3)).to(eq(6));
//	});
//
//	describe("mul", [] {
//		it("3 * -4 = -12", [] {
//			expect(mul(3, -4)).to(eq(-12));
//		});
//	});
});

} // namespace simple
