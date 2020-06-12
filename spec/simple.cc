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
describe("for 9x9 red-blue-white test file", [] {

	describe("decodes indexed rgba", [] {
		it("pixel[0]", [] {
			expect("p0").to(eq("p0"));
		});

	});
});

} // namespace simple
