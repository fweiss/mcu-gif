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

auto string_table_spec =
describe("string table", [] {
    static gd_string_table_t string_table;
    static gd_string2_t string;

    describe("initial", [&] {
        before("each", [&] {
            gd_string_table_init(&string_table);
        });
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
});

} // namespace simple
