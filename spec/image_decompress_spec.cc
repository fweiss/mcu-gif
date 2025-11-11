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

#include "helpers/allocateMemory.h"

namespace simple {

// testing at the level of gd_image_code_expand()
// add successive unpacked codes
// examine the output, string_table, prior string
//
// assumes string table is tested
// does not test unpacking

gd_expand_codes_t expand;

auto image_decompress_spec =
describe("image decompress", [] {

    before("each", [] {
        static gd_index_t output[512];
        expand.output = output;
        expand.outputLength = 0;
        expand.string_table.memory = allocate();

        // need to init expand
        expand.clearCode = 4;
        expand.codeSize = 3;

    });
    describe("clear code", [] {
        before("each", [] {
            // bogus values
            expand.prior_string.length = 1;
            expand.string_table.entries_length = 44;
            gd_image_code_expand(&expand, expand.clearCode);
        });
        it("resets prior string length", [] {
            expect(expand.prior_string.length).to(eq(0));
        });
        it("string table size + 2", [] {
            expect(expand.string_table.entries_length).to(eq(6));
        });
        it("string table entry matches index", [] {
            gd_string_table_entry_t &entry = expand.string_table.entries[2];
            expect(entry.length).to(eq(1));
            expect(expand.string_table.strings[entry.offset]).to(eq(2));
        });
    });
    describe("code stream [4, 2]", [] {
        before("each", [] {
            // todo also check bounds
            const gd_code_t code = 2;
            gd_image_code_expand(&expand, expand.clearCode);
            gd_image_code_expand(&expand, code);
        });
        describe("output", [] {
            it("length = 1", [] {
                expect(expand.outputLength).to(eq(1));
            });
            it("value = 2", [] {
                expect(expand.output[0]).to(eq(2));
            });
        });
        describe("previous code", [] {
            it("length = 1", [] {
                expect(expand.prior_string.length).to(eq(1));
            });
            it("value = 2", [&] {
                expect(expand.prior_string.value[0]).to(eq(2));
            });
        });
    });
    describe("second code", [] {
        before("each", [] {
            const gd_code_t firstcode = 1;
            gd_image_code_expand(&expand, expand.clearCode);
            gd_image_code_expand(&expand, firstcode);
        });
        describe("found expanding [4,1,2]", [] {
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
            describe("add to code table", [] {
                it("length", [] {
                    expect(expand.string_table.entries_length).to(eq(7));
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
            });
            describe("prior string", [] {
                it("length", [] {
                    expect(expand.prior_string.length).to(eq(1));
                });
                it("value[0]", [] {
                    expect(expand.prior_string.value[0]).to(eq(2));
                });
            });
        });
    });
    describe("code stream [4, 1, 6]", [] {
        before("each", [] {
            gd_image_code_expand(&expand, 4);
            gd_image_code_expand(&expand, 1);
            gd_image_code_expand(&expand, 6);
        });
        describe("output", [] {
            it("length = 3", [] {
                expect(expand.outputLength).to(eq(3));
            });
            it("indexes [1, 1, 1]", [] {
                expect(expand.output[0]).to(eq(1));
                expect(expand.output[1]).to(eq(1));
                expect(expand.output[2]).to(eq(1));
            });
        });
        describe("code table", [] {
            it("length", [] {
                expect(expand.string_table.entries_length).to(eq(7));
            });
            describe("entry [6]", [] {
                it("length = 2", [] {
                    gd_string_table_entry_t *entry = &expand.string_table.entries[6];
                    expect(entry->length).to(eq(2));
                });
                it("value[0] = 1", [] {
                    uint16_t offset = expand.string_table.entries[6].offset;
                    gd_index_t *value = &expand.string_table.strings[offset];
                    expect(value[0]).to(eq(1));
                });
                it("value[1] = 1", [] {
                    uint16_t offset = expand.string_table.entries[6].offset;
                    gd_index_t *value = &expand.string_table.strings[offset];
                    expect(value[1]).to(eq(1));
                });
            });
        });
        describe("prior string", [] {
            it("length = 2", [] {
                expect(expand.prior_string.length).to(eq(2));
            });
            it("code[0] = 1", [] {
                expect((int)expand.prior_string.value[0]).to(eq(1));
            });
            it("code[1] = 1", [] {
                expect((int)expand.prior_string.value[1]).to(eq(1));
            });
        });
    });
    describe("nth code", [] {});
});

} // namespace simple
