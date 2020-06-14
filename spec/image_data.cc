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

auto image_data_spec =
describe("image data", [] {
    uint8_t minimumCodeSize = 2;
    uint8_t input[8] = { 0x01, 0x02 };
    uint8_t output[1024]; // fixme max output length?

    describe("smallest block", [&] {
        uint16_t outputLength = gd_image_data_decode_block(minimumCodeSize, output);
        it("outputLength", [outputLength] {
            expect(outputLength).to(be == 2);
        });

    });
});

} // namespace simple
