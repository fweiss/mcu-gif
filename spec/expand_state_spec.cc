#include "ccspec/core.h"

#include "ccspec/expectation.h"
#include "ccspec/matchers.h"

using ccspec::core::describe;
using ccspec::core::before;
using ccspec::core::it;
using ccspec::expect;
using ccspec::matchers::eq;

#include "gd_internal.h"

namespace simple {

const size_t outputSize = 1024;  // fixme max output size for a sub block?

typedef std::vector<uint8_t> code_stream_t;

auto expand_state_spec =
describe("expand state", [] {
    describe("initial", [&] {
        it("code 4 clears", [&] {
            gd_expand_codes_t image;
            image.string_table.length = 10;
            gd_image_expand_code(&image, 4);
            expect(image.string_table.length).to(eq(6));
        });
    });
});

} // namespace
