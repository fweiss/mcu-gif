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

#include <queue>


namespace simple {

auto helpers_spec =
describe("helpers unpack", [] {

    it("does", [] {
        Unpack p;
//        p + 4 + 5;
        std::vector<uint8_t> unpacked = p + 4 + 5;
        // expect 0x2C
        expect(unpacked.size()).to(eq(1));
//        expect(unpacked.front()).to(eq(0x2C));
        expect((uint16_t)unpacked[0]).to(eq(0x2C));
    });
});

} // namespace
