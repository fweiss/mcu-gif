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

auto string_table_spec =
describe("string table", [] {
    static gd_string_table_t string_table;
    static gd_string_t string;

    // deprecated, shout not be internal static allocation
    // fixme remove magic constants
    describe("capacity", [&] {
        before("each", [&] {
            gd_string_table_init(&string_table, 2);
        });
        it("entries", [&] {
            expect(string_table.capacity).to(eq(564));
        });
        it("strings", [&] {
            expect(string_table.strings_capacity).to(eq(512));
        });
        it("init err", [&] {
            expect(string_table.status).to(eq(GD_X_OK));
        });
    });

    describe("smallest", [&] {
        before("each", [&] {
            string_table.status = GD_ERR_NO_INIT;
            gd_string_table_init(&string_table, 2);
        });

        describe("initial", [&] {
            it("has [0]", [&] {
                string = gd_string_table_at(&string_table, 0);
                expect(string.length).to(eq(1));
                expect(string.value[0]).to(eq(0x0000));
            });
            it("has [3]", [&] {
                string = gd_string_table_at(&string_table, 3);
                expect(string.length).to(eq(1));
                expect(string.value[0]).to(eq(0x0003));
            });
            it("has length 6", [&] {
                expect(string_table.length).to(eq(6));
            });
            it("does not have [6]", [&] {
                string = gd_string_table_at(&string_table, 6);
                expect(string.length).to(eq(0));
            });
            it("err ok", [&] {
                expect((int)string_table.status).to(eq(GD_X_OK));
            });
        });

        describe("add one", [&] {
            static gd_index_t raw_string[] = { 1, 6, 7, 30 };
            before("each", [&] {
                string.length = sizeof(raw_string) / sizeof(raw_string[0]);
                string.value = raw_string;
            });
            describe("out of space", [&] {
                it("entries", [&] {
                    string_table.capacity = 6;
                    uint16_t code = gd_string_table_add(&string_table, &string);
        //            expect(code).to(eq(0xFFFF));
                    expect((uint16_t)string_table.status).to(eq((uint16_t)GD_ERR_ENTRIES_NO_SPACE));
                });
                it("strings", [&] {
                    string_table.strings_capacity = 2;
                    uint16_t code = gd_string_table_add(&string_table, &string);
                    expect((uint16_t)string_table.status).to(eq((uint16_t)GD_ERR_STRINGS_NO_SPACE));
                });
            });
            it("return new code", [&] {
                uint16_t code = gd_string_table_add(&string_table, &string);
                expect((uint16_t)string_table.status).to(eq((uint16_t)GD_OK));
                expect(code).to(eq(6));
            });
        });
        describe("lookup", [&] {
            static gd_index_t raw_string[] = { 1, 6, 7, 30 };
            static gd_string_t string;
            before("each", [&] {
                string.length = sizeof(raw_string) / sizeof(raw_string[0]);
                string.value = raw_string;
                uint16_t code = gd_string_table_add(&string_table, &string);
                string = gd_string_table_at(&string_table, 6);
            });
            it("returns length", [&] {
                expect(string.length).to(eq(4));
            });
            it("returns value[2]", [&] {
                expect(string.value[2]).to(eq(7));
            });
        });
    });
    describe("largest code size 8", [&] {
        before("each", [&] {
            const uint8_t minimumCodeSize = 8;
            gd_string_table_init(&string_table, minimumCodeSize);
        });
        it("has size 256+2", [&] {
            expect(string_table.length).to(eq(258));
        });
        describe("entries", [&] {
            it("allocated 256+2 entries", [&] {
                expect((int)string_table.length).to(eq(256+2));
            });
        });
        describe("string", [&] {
            it("next string pos", [&] {
                expect((int)string_table.strings_length).to(eq(256));
            });
        });

        it("adds code beyond", [&] {
            expect(string_table.length).to(eq(256+2));
        });
        describe("string table entry[255]", [&] {
            it("has length 1", [&] {
                gd_string_table_entry_t &entry = string_table.entries[255];
                expect(entry.length).to(eq(1));
            });
            it("has index 255", [&] {
                gd_string_table_entry_t &entry = string_table.entries[255];
                gd_index_t *index = &string_table.strings[entry.offset];
                expect((int)index[0]).to(eq(255));
            });
            it("has value 255", [&] {
                gd_string_table_entry_t entry = string_table.entries[255];
                expect((int)string_table.strings[entry.offset]).to(eq(255));
            });
        });

        // deprecated, should be in gd_expand_t
        // it("clear code", [&] {
        //     expect(string_table.clearCode).to(eq(256));
        // });
        // it("end of info code", [&] {
        //     expect(string_table.endCode).to(eq(257));
        // });
    });


});

} // namespace simple
