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

#include "fake_file.h"

#include "gd.h"

namespace simple {

const size_t outputSize = 1024;  // fixme max output length?
static uint16_t output[outputSize];

auto image_data_spec =
describe("image data", [] {
//    uint8_t minimumCodeSize = 2;
    static gd_image_data_block_decode_t blockDecode;
    blockDecode.read = f_read;
    static uint16_t outputLength;

    before("each", [] {
        memset(output, 0, outputSize); // fixme 16 bit
    });

    describe("one sub block", [&] {
        describe("zero codes", [&] {
            before("each", [&] {
                // 1 byte of #4, $5, 1 byte EOB
                static uint8_t input[8] = { 0x01, 0x2C, 0x00 };
                FFILE(input);
                outputLength = gd_image_data_block_decode(&blockDecode, output);
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
                outputLength = gd_image_data_block_decode(&blockDecode, output);
            });
            it("output length", [&] {
                expect(outputLength).to(be == 1);
            });
            it("output[0]", [&] {
                expect(output[0]).to(eq(1));
            });
        });
    });
});

} // namespace simple
