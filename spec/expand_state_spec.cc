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
        before("each", [] {
            gd_image_expand_code(&expand, 4);
        });
        describe("prior string was ()", [] {
            before("each", [] {
                expand.prior_string.length = 0;
            });
            describe("found 2 (2)", []  {
                before("each", [] {
                    gd_image_expand_code(&expand, 2);
                });
                it("prior length 1", [] {
                     expect(expand.prior_string.length).to(eq(1));
                });
                it("prior value[0] 2", [] {
                     expect(expand.prior_string.value[0]).to(eq(2));
                });
            });
            describe("not found", [] {
                it("undefined", [] {});
            });
        });
        describe("prior string was (3)", [] {
            static uint16_t string[] = { 3 };
            before("each", [] {
                expand.prior_string.length = 1;
                expand.prior_string.value = string;
            });
            describe("found 2 {2}", []  {
                before("each", [] {
                    gd_image_expand_code(&expand, 2);
                });
                it("prior length 1", [] {
                     expect(expand.prior_string.length).to(eq(1));
                });
                it("prior value[0] 2", [] {
                     expect(expand.prior_string.value[0]).to(eq(2));
                });
            });
            describe("not found 6 ()", [] {
                before("each", [] {
                    gd_image_expand_code(&expand, 6);
                });
                it("prior length 2", [] {
                     expect(expand.prior_string.length).to(eq(2));
                });
                it("prior value[0] 3", [] {
                     expect(expand.prior_string.value[0]).to(eq(3));
                });
                it("prior value[1] 3", [] {
                     expect(expand.prior_string.value[1]).to(eq(3));
                });
            });
        });
        describe("prior string (1,2)", [] {
            before("each", [] {
                static uint16_t value[] = { 1, 2 };
                expand.prior_string.length = 2;
                expand.prior_string.value = value;
            });
            describe("found 3 (3)", [] {
                before("each", [] {
                    gd_image_expand_code(&expand, 3);
                });
                it("prior length 1", [] {
                    expect(expand.prior_string.length).to(eq(1));
                });
                it("prior value[0] 3", [] {
                    expect(expand.prior_string.value[0]).to(eq(3));
                });
            });
            describe("found 6 (0,3)", [] {
                before("each", [] {
                    static uint16_t string_value[] = { 0, 3 };
                    gd_string2_t string;
                    string.length = 2;
                    string.value = string_value;
                    // assume this will add code 6
                    gd_string_table_add(&expand.string_table, &string);
                    gd_image_expand_code(&expand, 6);
                });
                it("prior length 2", [] {
                    expect(expand.prior_string.length).to(eq(2));
                });
                it("prior value[0] 1", [] {
                    expect(expand.prior_string.value[0]).to(eq(0));
                });
                it("prior value[1] 2", [] {
                    expect(expand.prior_string.value[1]).to(eq(3));
                });
            });
            describe("not found 7", [] {
                before("each", [] {
                    gd_image_expand_code(&expand, 7);
                });
                it("prior length 3", [] {
                    expect(expand.prior_string.length).to(eq(3));
                });
                it("prior value[0] 1", [] {
                    expect(expand.prior_string.value[0]).to(eq(1));
                });
                it("prior value[1] 2", [] {
                    expect(expand.prior_string.value[1]).to(eq(2));
                });
                it("prior value[2] 1", [] {
                    expect(expand.prior_string.value[2]).to(eq(1));
                });
            });
        });
    });
});

} // namespace
