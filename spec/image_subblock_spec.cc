#include "ccspec/core.h"

#include "ccspec/expectation.h"
#include "ccspec/matchers.h"

using ccspec::core::describe;
using ccspec::core::before;
using ccspec::core::it;
using ccspec::expect;
using ccspec::matchers::eq;

#include "helpers/fake_file.h"
#include "helpers/pack.h"
#include "helpers/allocateMemory.h"

extern "C" {
	#include "gd.h"
	#include "gd_internal.h"
}

namespace simple {

const size_t outputSize = 1024;  // fixme max output size for a sub block?

static Pack p;

// initialize the unpack object
static void unpackInit(gd_unpack_t *unpack) {
    unpack->expandCodes.outputLength = 0;
    unpack->onDeckBits = 0;
    unpack->onDeck = 0;
    unpack->codeBits = 3;
    unpack->codeMask = 0x07;
    unpack->expandCodes.string_table.strings_length = 0;
    unpack->expandCodes.string_table.entries_length = 0;
    p.reset();
}

// static storage and internal linkage
// allows setting global watch expressions in debugger
static gd_unpack_t unpack;

typedef std::vector<uint8_t> code_stream_t;

auto image_subblock_spec =
describe("image subblock with", [] {

    static gd_index_t output[outputSize];

    static gd_image_block_t block;

    static gd_expand_codes_t expand;

    // block.expand_codes.string_table.memory = allocate();

    before("all", [&] {
        // N.B. 'output' must be the array, not a pointer
        memset(output, 0, sizeof(output));
        p.reset();
        block.output = output;
        block.outputLength = 0;

        unpack.codeBits = 3;
        unpack.codeMask = 0x07;
        unpack.extract = 0; // init?
        unpack.onDeck= 0; // init?
        unpack.onDeckBits = 0; // imit?
        unpack.top = 0; // init?
        unpack.topBits = 0; // init?

        unpack.expandCodes.output = output;
        unpack.expandCodes.outputCapacity = sizeof(output);
        unpack.expandCodes.codeSize = 3;
        unpack.expandCodes.clearCode = 4;
        unpack.expandCodes.endCode = 5;
        unpack.expandCodes.string_table.memory = allocate();
        unpack.expandCodes.minumumCodeSize = 2;

    });
    describe("codes 4 + 5", [&] {

        before("all", [&] {
            PackedSubBlock packed = p + 4 + 5;
            gd_image_subblock_unpack(&unpack, packed.data(), packed.size());
        });

        it("output length", [&] {
            expect(expand.outputLength).to(eq(0));
        });
        it("4 entries", [] {
            expect(unpack.expandCodes.string_table.entries_length).to(eq(4 + 2));
        });
    });
    describe("codes 4 + 0 + 5", [] {

        before("all", [&] {
            p.reset();
            PackedSubBlock packed = p + 4 + 0 + 5;
            unpack.onDeckBits = 0;
                    unpack.expandCodes.outputLength = 0; // init?

            gd_image_subblock_unpack(&unpack, packed.data(), packed.size());
        });

        it("output length", [&] {
            expect(unpack.expandCodes.outputLength).to(eq(1));
        });

        it("[0]", [&] {
            expect(unpack.expandCodes.output[0]).to(eq(0x00));
        });
    });
    describe("codes 4 + 1 + 5", [&] {
        before("all", [] {
            unpackInit(&unpack);
            PackedSubBlock packed = p + 4 + 1 + 5;

            gd_image_subblock_unpack(&unpack, packed.data(), packed.size());
        });
        it("output length", [] {
            expect(unpack.expandCodes.outputLength).to(eq(1));
        });
        it("[0]", [] {
            expect(unpack.expandCodes.output[0]).to(eq(0x0001));
        });
    });

    describe("codes 4 + 0 + 1 + 5", [&] {
        before("each", [&] {
            unpackInit(&unpack);
            PackedSubBlock packed = p + 4 + 0 + 1 + 5;

            gd_image_subblock_unpack(&unpack, packed.data(), packed.size());
        });
        it("output length", [&] { 
            expect(unpack.expandCodes.outputLength).to(eq(2));
        });
        it("[0]", [&] {
            expect(unpack.expandCodes.output[1]).to(eq(0x01));
        });
    });

    describe("code size increases", [&] {
        before("all", [] {
            unpackInit(&unpack);
            // based on a priori knowledge that first #2 will enlarge code table
            // to require 4 bit codes
            PackedSubBlock packed = p + 4 + 1 + 6 + 6 + Shift(4) + 2 + 5;
            block.outputLength = 0;

            gd_image_subblock_unpack(&unpack, packed.data(), packed.size());
        });
        it("to 4 bits", [&] {
            expect(unpack.expandCodes.codeSize).to(eq(4));
        });
    });
    // to ensure that extract is not propagated:
    //   codeBits > onDeckBits
    // to ensure that block is not read:
    //  topBits == 0 && onDeckBits < codeBits
    describe("shift top to onDeck", [] {
        before("all", [] {
            unpackInit(&unpack);
            unpack.codeBits = 17; // force exit
            unpack.codeMask = 0; // NA
            unpack.onDeck = 0b0000000111111111;
            unpack.onDeckBits = 9;
            unpack.top = 0b01111111;
            unpack.topBits = 7;
            // const uint8_t packed[0]; // nothing is read
            gd_image_subblock_unpack(&unpack, (uint8_t *)NULL, 0);
        });
        it("verify onDeck", [] {
            expect(unpack.onDeck).to(eq(0b1111111111111111));
        });
        it("verify top", [] {
            expect(unpack.top).to(eq(0));
        });
    });
    describe("code size increase", [] {
        describe("from 9 bits", [] {
            before("all", [] {
                unpackInit(&unpack);
                unpack.codeBits = 9;
                unpack.codeMask = 0x1ff;
                // one less that what would trigger a resize
                unpack.expandCodes.string_table.entries_length = 0x1ff;
                unpack.expandCodes.endCode = 0x101;
                unpack.expandCodes.compressStatus = 1;
                uint8_t p[] = { 0b00000010, 0b00000011, 0b00000010, }; // 0x102, 0x101/end
                gd_image_subblock_unpack(&unpack, p, sizeof(p));
            });
            it("to 10 bits", [] {
                expect(unpack.codeBits).to(eq(10));
            });
            // it("string table length", [] {
            //     expect()
            // });
        });
    });
    describe("clear code", [] {
        before("all", [] {
            const uint16_t clearCode = unpack.expandCodes.clearCode;
            p.reset();
            unpack.expandCodes.string_table.entries_length = 0;
            block.outputLength = 0;

            unpackInit(&unpack);
            const uint16_t endCode = 5;
            PackedSubBlock packed = p + 4 + 1 + 6 + 6 + Shift(4) + 2 + clearCode + endCode;
            unpack.expandCodes.codeSize = 2;
            unpack.expandCodes.minumumCodeSize = 2;
            gd_image_subblock_unpack(&unpack, packed.data(), packed.size());
        });
        it("resets entries table", [] {
            const uint16_t entries = 4 + 2; //codes plus clear/end
            expect(unpack.expandCodes.string_table.entries_length).to(eq(entries));
        });
        it("resets strings table", [] {
            const uint16_t strings = 4;
            expect(unpack.expandCodes.string_table.strings_length).to(eq(strings));
        });
        it("resets code size", [] { // codebits/codesize?
            expect(unpack.codeBits).to(eq(3)); // min + 1
        });
    });
    // full 10x10 reference example
    // #4 #1 #6 #6 #2 #9 #9 #7 #8 #10 #2 #12 #1 #14 #15 #6 #0 #21 #0 #10 #7 #22 #23 #18 #26 #7 #10 #29 #13 #24 #12 #18 #16 #36 #12 #5
    // 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, ...
});

} // namespace simple
