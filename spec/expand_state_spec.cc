#include "ccspec/core.h"

#include "ccspec/expectation.h"
#include "ccspec/matchers.h"

using ccspec::core::describe;
using ccspec::core::before;
using ccspec::core::it;
using ccspec::expect;
using ccspec::matchers::eq;

#include <string>

#include "gd_internal.h"

namespace simple {

const size_t outputSize = 1024;  // fixme max output size for a sub block?

auto expand_state_spec =
describe("expand state", [] {

    static gd_expand_codes_t expand;
    static uint16_t output[outputSize];

    before("each", [] {
        memset(output, 0, sizeof(output));
        expand.codeSize = 3;
        expand.output = output;
        expand.outputLength = 0;

    });

    describe("initial", [&] {

        it("code 4 clears", [&] {
            expand.string_table.length = 10;
            gd_image_expand_code(&expand, 4);
            expect(expand.string_table.length).to(eq(6));
        });
    });
    describe("scenario", [] {
        describe("prior string ()", [] {
            describe("found (2)", []  {
                before("each", [] {
                    gd_image_expand_code(&expand, 4);
                    expand.string.length = 0;
                    gd_image_expand_code(&expand, 2);
                });
                it("prior length", [] {
                     expect(expand.string.length).to(eq(1));
                });
                it("prior value[0]", [] {
                     expect(expand.string.value[0]).to(eq(2));
                });
            });
        });
        describe("first", [&] {
            before("each", [] {
                    gd_image_expand_code(&expand, 4);
            });
            describe("found code", [&] {
                before("each", [&] {
                    gd_image_expand_code(&expand, 2);
                });
                it("prior code", [] {
                    expect(expand.string.length).to(eq(1));
                    expect(expand.string.value[0]).to(eq(2));
                });
            });
    // behavior unspecified
            describe("not found code", [] {
    //            before("each", [&] {
    //                gd_image_expand_code(&expand, 6);
    //            });
    //            it("prior code", [] {
    //                expect(expand.string.length).to(eq(1));
    //            });
            });
        });
        describe("second", [] {
            before("each", [] {
                gd_image_expand_code(&expand, 4);
                gd_image_expand_code(&expand, 2);
                gd_image_expand_code(&expand, 6);
            });
            it("prior code", [] {
                expect(expand.string.length).to(eq(2));
                expect(expand.string.value[0]).to(eq(2));
                expect(expand.string.value[1]).to(eq(2));
            });
        });
        describe("third", [] {
            before("each", [] {
                gd_image_expand_code(&expand, 4);
                gd_image_expand_code(&expand, 3);
                gd_image_expand_code(&expand, 6);
                gd_image_expand_code(&expand, 6);
            });
            it("prior code", [] {
                expect(expand.string.length).to(eq(2));
                expect(expand.string.value[0]).to(eq(3));
                expect(expand.string.value[1]).to(eq(3));
            });
        });
    });

});

} // namespace
