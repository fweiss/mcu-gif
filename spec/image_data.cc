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

auto image_data_spec =
describe("image data", [] {
//    uint8_t minimumCodeSize = 2;

    static uint16_t output[outputSize];

    static gd_image_data_block_decode_t blockDecode;
    blockDecode.read = f_read;
    static uint16_t outputLength;
    static Pack p;

    before("each", [] {
        // N.B. 'output' must be the array, not a pointer
        memset(output, 0, sizeof(output));
        p.reset();
    });

    describe("one sub block", [&] {

        describe("zero codes", [&] {
            before("each", [&] {
                // 1 byte of #4, $5, 1 byte EOB
                static uint8_t input[8] = { 0x01, 0x2C, 0x00 };
                FFILE(input);
                outputLength = gd_image_sub_block_decode(&blockDecode, output);
            });
            it("output length", [&] {
                expect(outputLength).to(be == 0);
            });
        });

        describe("one code", [&] {
            before("each", [&] {
                // #4 #1 #5 EOB
                static uint8_t input[] = { 0x02, 0x4C, 0x01, 0x00 };
                FFILE(input);
                outputLength = gd_image_sub_block_decode(&blockDecode, output);
            });
            it("output length", [&] {
                expect(outputLength).to(be == 1);
            });
            it("output[0]", [&] {
                expect(output[0]).to(eq(1));
            });
        });

        describe("try Pack", [&] {
            it("simple", [&] {
                std::vector<uint8_t> packed = p + 4 + 1 + 5;
                packed.insert(packed.begin(), packed.size());
                f_open_memory(packed.data(), packed.size());
                outputLength = gd_image_sub_block_decode(&blockDecode, output);
                expect(outputLength).to(eq(1));
                expect(output[0]).to(eq(1));
            });
        });

        // full 10x10 reference example
        // #4 #1 #6 #6 #2 #9 #9 #7 #8 #10 #2 #12 #1 #14 #15 #6 #0 #21 #0 #10 #7 #22 #23 #18 #26 #7 #10 #29 #13 #24 #12 #18 #16 #36 #12 #5
        // 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, 1, 1, 1, 0, 0, 0, 0, 2, 2, 2, ...
    });
});

} // namespace simple
