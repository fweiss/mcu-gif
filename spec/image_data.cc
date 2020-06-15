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
    gd_image_data_block_decode_t blockDecode;
    blockDecode.read = f_read;

    before("each", [] {
        memset(output, 0, outputSize);
    });

    describe("zero block", [&] {
        // 1 byte of #4, $5, 1 byte EOB
        uint8_t input[8] = { 0x01, 0x2C, 0x00 };
        FFILE(input);
        uint16_t outputLength = gd_image_data_block_decode(&blockDecode, output);
        it("outputLength", [outputLength] {
            expect(outputLength).to(be == 0);
        });
    });

    describe("one block", [&] {
        uint8_t input[] = { 0x02, 0x4C, 0x01, 0x00 };
        FFILE(input);
        uint16_t outputLength = gd_image_data_block_decode(&blockDecode, output);
        it("outputLength", [outputLength] {
            expect(outputLength).to(be == 1);
//            expect(output[0]).to(eq(1));
        });
    });
});

} // namespace simple
