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

#include "helpers/pack.h"

#include <string>
#include <cstdio>

// for comparing vectors, until ccspec has a diff reporter
static std::string dump(std::vector<uint8_t> p) {
    std::string o;
    for (auto b : p) {
        char buf[3];
        sprintf(buf, "%02X", b);
        o += buf;
        o += ",";
    }
    return o.c_str();
}

namespace simple {

auto helpers_spec =
describe("helpers pack", [] {
    static Pack p;
    static std::vector<uint8_t> packed;

    before("each", [&] {
        p.reset();
    });

    describe("4 + 5", [&] {

        before("each", [&] {
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
            packed = p + 4 + 1 + 5;
        });

        it("size", [] {
            expect(packed.size()).to(eq(2));
        });

        it("[1]", [] {
            expect((uint16_t)packed[1]).to(eq(0x01));
        });
    });

    // 101 110 001 100 = 1011 1000 1100
    describe("4 + 1 + 6 + 5", [&] {

        before("each", [&] {
            packed = p + 4 + 1 + 6 + 5;
        });

        it("size", [&] {
            expect(packed.size()).to(eq(2));
        });

        it("[2]", [&] {
            expect((uint16_t)packed[1]).to(eq(0xB));
        });

        it("all", [&] {
            expect(dump(packed)).to(eq("8C,0B,"));
        });
    });

    // 101 110 110 001 100 = 101 1101 1000 1100
    describe("4 + 1 + 6 + 6 + 5", [&] {

        before("each", [&] {
            packed = p + 4 + 1 + 6 + 6 + 5;
        });

        it("all", [&] {
            expect(dump(packed)).to(eq("8C,5D,"));
        });
    });

    // 101 010 110 110 001 100 = 10 1010 1101 1000 1100
    describe("4 + 1 + 6 + 6 + 2 + 5", [&] {

        before("each", [&] { packed = p + 4 + 1 + 6 + 6 + 2 + 5; });

        it("all", [&] { expect(dump(packed)).to(eq("8C,AD,02,")); });
    });

    // 0101 1001 0010 0110 110 001 100 = 0 1011 0010 0100 1101 1000 1100
    describe("4, 1, 6, Shift(4) 6, 2, 9 (4 code bits)", [] {

        before("each", [] {
            packed = p + 4 + 1 + 6 + Shift(4) + 6  + 2 + 9 + 5;
        });

        it("all", [&] {
            expect(dump(packed)).to(eq("8C,4D,B2,00,"));
        });
    });

});

} // namespace
