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

    before("each", [&] {
        gd_string_table_init(&string_table);
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
                expect((uint16_t)string_table.status).to(eq((uint16_t)GD_ERROR));
            });
            it("strings", [&] {
                string_table.strings_capacity = 2;
                uint16_t code = gd_string_table_add(&string_table, &string);
                expect((uint16_t)string_table.status).to(eq((uint16_t)GD_ERROR));
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

} // namespace simple
