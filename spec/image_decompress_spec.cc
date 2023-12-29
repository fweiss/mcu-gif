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
    describe("second code", [] {
        before("each", [] {
            const gd_code_t firstcode = 1;
            gd_image_code_expand(&expand, 4);
            gd_image_code_expand(&expand, firstcode);
        });
        describe("found", [] {
            before("each", [] {
                const gd_code_t secondcode = 2;
                gd_image_code_expand(&expand, secondcode);
            });
            describe("output", [] {
                it("length", [] {
                    expect(expand.outputLength).to(eq(2));
                });
                it("value", [] {
                    expect(expand.output[1]).to(eq(2));
                });
            });
            describe("add to code table", [] {});
                it("length", [] {
                    expect(expand.string_table.length).to(eq(7));
                });
                describe("entry", [] {
                    // gd_string_table_entry_t entry = expand.string_table.entries[7];
                    it("length", [] {
                        gd_string_table_entry_t &entry = expand.string_table.entries[6];
                        expect(entry.length).to(eq(2));
                    });
                    it("value[0]", [] {
                        gd_string_table_entry_t &entry = expand.string_table.entries[6];
                        size_t offset = entry.offset;
                        expect(expand.string_table.strings[offset]).to(eq(1));
                    });
                    it("value[1]", [] {
                        gd_string_table_entry_t &entry = expand.string_table.entries[6];
                        size_t offset = entry.offset;
                        expect(expand.string_table.strings[offset + 1]).to(eq(2));
                    });
                });
            describe("previous code", [] {});
        });
        describe("not found", [] {
            before("each", [] {
                const gd_code_t secondcode = 6;
                gd_image_code_expand(&expand, secondcode);
            });
            describe("output", [] {
                it("length", [] {
                    expect(expand.outputLength).to(eq(3));
                });
                it("output[1]", [] {
                    expect(expand.output[1]).to(eq(1));
                });
                it("output[2]", [] {
                    expect(expand.output[1]).to(eq(1));
                });
            });
            describe("add to code table", [] {
                it("length", [] {
                    expect(expand.string_table.length).to(eq(7));
                });
                describe("entry", [] {
                    // gd_string_table_entry_t *entry;
                    auto entries = [] (int y) -> gd_string_table_entry_t& {
                        return expand.string_table.entries[y];
                    };

                    before("each", [] {                    
                        // entry = &expand.string_table.entries[7];
                    });
                    it("length", [] {
                        gd_string_table_entry_t entry = expand.string_table.entries[6];
                        expect(entry.length).to(eq(2));
                    });
                    it("value[0]", [&entries] {
                        int offset = entries(6).offset;
                        expect(expand.string_table.strings[offset]).to(eq(1));
                    });
                    it("value[1]", [&entries] {
                        gd_index_t * strings = expand.string_table.strings;
                        expect(strings[entries(6).offset + 1]).to(eq(1));
                    });
                });
            });
            describe("previous code", [] {
                it("length", [] {
                    expect(expand.prior_string.length).to(eq(1));
                });
                it("code[0]", [] {
                    expect(expand.prior_string.value[0]).to(eq(6));
                });
            });
        });
    });
    describe("nth code", [] {});
});

} // namespace simple
