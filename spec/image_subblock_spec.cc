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

#include "helpers/fake_file.h"
#include "helpers/pack.h"

#include "gd.h"

namespace simple {

const size_t outputSize = 1024;  // fixme max output size for a sub block?

typedef std::vector<uint8_t> code_stream_t;

auto image_subblock_spec =
describe("image subblock with", [] {
//    uint8_t minimumCodeSize = 2;

    static uint16_t output[outputSize];

    static gd_image_block_t block;
    static uint16_t outputLength;
    static Pack p;

    before("each", [] {
        // N.B. 'output' must be the array, not a pointer
        memset(output, 0, sizeof(output));
        p.reset();
        block.output = output;
        block.outputLength = 0;
    });

    describe("codes 4 + 5", [&] {

        before("each", [&] {
            code_stream_t packed = p + 4 + 5;
            gd_image_subblock_decode(&block, packed.data(), packed.size());
        });

        it("output length", [&] { expect(block.outputLength).to(eq(0)); });
    });

    describe("codes 4 + 0 + 5", [&] {

        before("each", [&] {
            code_stream_t packed = p + 4 + 0 + 5;
            gd_image_subblock_decode(&block, packed.data(), packed.size());
        });

        it("output length", [&] { expect(block.outputLength).to(eq(1)); });

        it("[0]", [&] { expect(block.output[0]).to(eq(0x00)); });
    });

    describe("codes 4 + 1 + 5", [&] {

        before("each", [&] {
            code_stream_t packed = p + 4 + 1 + 5;
            gd_image_subblock_decode(&block, packed.data(), packed.size());
        });

        it("output length", [&] { expect(block.outputLength).to(eq(1)); });

        it("[0]", [&] { expect(block.output[0]).to(eq(0x0001)); });
    });

    describe("codes 4 + 0 + 1 + 5", [&] {

        before("each", [&] {
            code_stream_t packed = p + 4 + 0 + 1 + 5;
            gd_image_subblock_decode(&block, packed.data(), packed.size());
        });
        it("output length", [&] { expect(block.outputLength).to(eq(2)); });
        it("[0]", [&] { expect(block.output[1]).to(eq(0x01)); });
    });

    describe("code size increases", [&] {
        it("to 4 bits", [&] {
            // based on a priori knowledge that first #6 will enlarge code table
            // to require 4 bit codes
            code_stream_t packed = p + 4 + 1 + 6 + Shift(4) + 6 + 5;
            block.outputLength = 0;
            gd_image_subblock_decode(&block, packed.data(), packed.size());
            expect(block.outputLength).to(eq(3));
        });
    });

    // full 10x10 reference example
    // #4 #1 #6 #6 #2 #9 #9 #7 #8 #10 #2 #12 #1 #14 #15 #6 #0 #21 #0 #10 #7 #22 #23 #18 #26 #7 #10 #29 #13 #24 #12 #18 #16 #36 #12 #5
    // 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, ...
});

} // namespace simple
