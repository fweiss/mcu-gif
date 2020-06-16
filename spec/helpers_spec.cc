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
    static std::vector<uint8_t> packed;

    describe("4 + 5", [&] {

        before("each", [&] {
            p.reset();
            packed = p + 4 + 5;
        });

        it("size", [] {
            expect(packed.size()).to(eq(1));
        });

        it("[0]", [] {
            expect((uint16_t)packed[0]).to(eq(0x2C));
        });
    });

    describe("4 + 1 + 5", [] {

        before("each", [] {
            p.reset();
            packed = p + 4 + 1 + 5;
        });

        it("size", [] {
            expect(packed.size()).to(eq(2));
        });

        it("[1]", [] {
            expect((uint16_t)packed[1]).to(eq(0x08));
        });
    });

    // 8C 2D 99 87
    describe("4, 1, 6, 6, 2, 9 (4 code bits)", [] {
        before("each", [] {
            p.reset();
//            packed = p + 4 + 1 + 6 + Shift(4) + 2 + 9 + 5;
        });
    });

});

} // namespace
