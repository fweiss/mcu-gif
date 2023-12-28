#include "ccspec/core.h"
#include "ccspec/expectation.h"
#include "ccspec/matchers.h"

using ccspec::core::describe;
using ccspec::core::before;
using ccspec::core::it;
using ccspec::expect;
using ccspec::matchers::eq;

extern "C" {
	#include "gd_internal.h"
}

namespace simple {

gd_expand_codes_t expand;

auto image_decompress_spec =
describe("image decompress", [] {

    before("each", [] {
        static gd_index_t output[512];
        expand.output = output;
        expand.outputLength = 0;

        // need to init expand
        expand.clearCode = 4;
        expand.codeSize = 3;

    });
    it("works", [] {
        expect(1+1).to(eq(2));
    });
    describe("first code", [] {
        before("each", [] {
            // todo also check bounds
            const gd_code_t code = 2;
            gd_image_code_expand(&expand, 4);
            gd_image_code_expand(&expand, code);
        });
        describe("output", [] {
            it("length", [] {
                expect(expand.outputLength).to(eq(1));
            });
            it("value", [] {
                expect(expand.output[0]).to(eq(2));
            });
        });
        describe("previous code", [] {
            it("length", [] {
                expect(expand.prior_string.length).to(eq(1));
            });
            it("value", [&] {
                expect(*expand.prior_string.value).to(eq(2));
            });
        });
    });
});

} // namespace simple
