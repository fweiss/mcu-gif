#include "ccspec/core.h"
#include "ccspec/expectation.h"
#include "ccspec/matchers.h"

using ccspec::core::describe;
using ccspec::core::before;
using ccspec::core::it;
using ccspec::expect;
using ccspec::matchers::be;
using ccspec::matchers::eq;
using ccspec::matchers::be_truthy;

#include "helpers.h"

namespace simple {

auto helpers_spec =
describe("helpers pack", [] {
    static Pack p;
    static std::vector<uint8_t> unpacked;

    describe("4 + 5", [&] {

        before("each", [&] {
            p.reset();
            unpacked = p + 4 + 5;
        });

        it("size", [] {
            expect(unpacked.size()).to(eq(1));
        });

        it("[0]", [] {
                expect((uint16_t)unpacked[0]).to(eq(0x2C));
        });
    });

});

} // namespace
