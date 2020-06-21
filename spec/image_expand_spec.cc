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

#include "gd.h"

namespace simple {

const size_t outputSize = 1024;  // fixme max output size for a sub block?

typedef std::vector<uint8_t> code_stream_t;

auto image_expand_spec =
describe("image expand", [] {

    static uint16_t output[outputSize];

    static gd_image_block_t block;
    static uint16_t outputLength;

    before("each", [] {
        // N.B. 'output' must be the array, not a pointer
        memset(output, 0, sizeof(output));
        block.output = output;
        block.outputLength = 0;
    });

    describe("simaple", [&] {
        it("works", [&] {
            gd_image_expand_code(&block, 0x0004);
            gd_image_expand_code(&block, 0x0000);
            gd_image_expand_code(&block, 0x0005);
            expect(block.outputLength).to(eq(1));
            expect(block.output[0]).to(eq(0));
        });
    });
});

} // namespace simple
