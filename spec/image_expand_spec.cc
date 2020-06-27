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
#include "gd_internal.h"

#include <vector>
#include <functional>

namespace simple {

const size_t outputSize = 1024;  // fixme max output size for a sub block?

typedef std::vector<uint8_t> code_stream_t;

auto image_expand_spec =
describe("image expand", [] {

    static uint16_t output[outputSize];

    static gd_expand_codes_t expand;
    static uint16_t outputLength;

    // clever use of lambda instead of define
    auto expand_codes_stream = [&] (std::vector<uint16_t> codes) {
        for (uint16_t code : codes) {
            gd_image_expand_code(&expand, code);
        }
    };

    before("each", [] {
        // N.B. 'output' must be the array, not a pointer
        memset(output, 0, sizeof(output));
        expand.codeSize = 3;
        expand.output = output;
        expand.outputLength = 0;
    });

    // 1's will output index one-for-one
    describe("simple uncompressed", [&] {
        describe("code sequence 8 1's", [&] {
            before("each", [&] {
                expand_codes_stream({ 4, 1, 1, 1, 1, 1, 1, 1, 1, 5});
            });
            it("output count", [&] {
                expect(expand.outputLength).to(eq(8));
            });
            it("output [1]", [&] {
                expect(expand.output[1]).to(eq(1));
            });
            it("output [7]", [&] {
                expect(expand.output[7]).to(eq(1));
            });
            it("code size", [&] {
                expect((uint16_t)expand.codeSize).to(eq(4));
            });
        });
        describe("code sequence 12 1's", [&] {
            before("each", [&] {
                expand_codes_stream({ 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5});
            });
            it("code size", [&] {
                expect((uint16_t)expand.codeSize).to(eq(5));
            });
        });
        describe("code sequence 28 1's", [&] {
            before("each", [&] {
                gd_image_expand_code(&expand, 4);
                for (int i=0; i<28; i++) {
                    gd_image_expand_code(&expand, 1);
                }
                gd_image_expand_code(&expand, 5);
            });
            it("code size", [&] {
                expect((uint16_t)expand.codeSize).to(eq(6));
            });
        });
    });

    describe("simple", [&] {
        it("works", [&] {
            expand_codes_stream({ 4, 0, 5 });
            expect(expand.outputLength).to(eq(1));
            expect(expand.output[0]).to(eq(0));
            expect((short)expand.codeSize).to(eq(3));
        });
    });
    describe("code size changed", [&] {
        it("to 4", [&] {
//            expand_codes_stream({ 4, 1, 6, 6, 2, 5 });
            uint16_t codes[] = { 4, 1, 6, 6, 2, 5};
            for (auto code : codes) {
                gd_image_expand_code(&expand, code);
            }
//            expect(expand.string_table.length).to(eq(6));
            expect((uint16_t)expand.codeSize).to(eq(4));
        });
    });

    describe("full example", [&] {
        before("each", [&] {
//            expand_codes_stream({ 4, 1, 6, 6, 2, 9, 9, 7, 8, 10, 2, 12, 1, 14, 15, 6, 0, 21, 0, 10, 7, 22, 23, 18, 26, 7, 10, 29, 13, 24, 12, 18, 16, 36, 12, 5});
            uint16_t codes[] = { 4, 1, 6, 6, 2, 9, 9, 7, 8, 10, 2, 12, 1, 14, 15, 6, 0, 21, 0, 10, 7, 22, 23, 18, 26, 7, 10, 29, 13, 24, 12, 18, 16, 36, 12, 5};
            for (auto code : codes) {
                gd_image_expand_code(&expand, code);
            }
        });
        it("output count", [&] {
            expect(expand.outputLength).to(eq(100));
        });
    });


//    describe("code table", [&] {
//        describe("initial", [&] {
//            before("each", [&] {
//                expand_codes_stream({ 4 });
//            });
//            it("size", [&] {
//                expect(expand.codeTableSize).to(eq(6));
//            });
//            it("[0]", [&] {
//                gd_string_t string = expand.codeTable[0];
//                expect(string.size).to(eq(1));
//                expect(string.data[0]).to(eq(0));
//            });
//        });
//        it("adds one", [&] {
//            expand_codes_stream({ 4, 1, 6 });
//            expect(expand.codeTableSize).to(eq(7));
//        });
//        it("adds two", [&] {
//            expand_codes_stream({ 4, 1, 6, 7 });
//            expect(expand.codeTableSize).to(eq(8));
//        });
//    });
});

} // namespace simple
