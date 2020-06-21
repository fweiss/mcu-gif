#include "ccspec/core.h"

#include "ccspec/expectation.h"
#include "ccspec/matchers.h"

using ccspec::core::describe;
using ccspec::core::before;
using ccspec::core::it;
using ccspec::expect;
using ccspec::matchers::eq;

#include "helpers/fake_file.h"

#include "gd.h"

namespace simple {

const size_t outputSize = 1024;  // fixme max output size for a sub block?

typedef std::vector<uint8_t> code_stream_t;

auto image_block_spec =
describe("image data block", [] {

    static uint16_t output[outputSize];

    static gd_main_t main;
    main.read = f_read;
    main.image_block.output = output;
    static gd_image_block_t block;

    before("each", [] {
        // N.B. 'output' must be the array, not a pointer
        memset(output, 0, sizeof(output));
    });

    describe("empty", [&] {
        before("each", [&] {
            static uint8_t input[] = { 0x02, 0x00 };
            FFILE(input);
            gd_image_block_read(&main, &block);
        });
        it("output length", [&] {
            expect(main.image_block.outputLength).to(eq(0));
        });
    });

    describe("minimum code size 2", [&] {
        before("each", [&] {
            static uint8_t input[] = { 0x02, 0x00 };
            FFILE(input);
            gd_image_block_read(&main, &block);
        });
        it("value", [&] {
            expect(main.image_block.minumumCodeSize).to(eq(2));
        });
    });

    describe("minimum code size 8", [&] {
        before("each", [&] {
            static uint8_t input[] = { 0x08, 0x00 };
            FFILE(input);
            gd_image_block_read(&main, &block);
        });
        it("value", [&] {
            expect(main.image_block.minumumCodeSize).to(eq(8));
        });
    });

    describe("one sub block", [&] {

        describe("basic", [&] {
            before("each", [&] {
                static uint8_t input[] = { 0x02, 0x02, 0x4C, 0x01, 0x00 };
                FFILE(input);
                gd_image_block_read(&main, &block);
            });
            it("output length", [&] {
                expect(main.image_block.outputLength).to(eq(1));
            });
            it("has one index", [&] {
                expect(main.image_block.output[0]).to(eq(1));
            });
        });
    });
});

} // namespace simple
