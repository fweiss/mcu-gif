#include "ccspec/core.h"
#include "ccspec/expectation.h"
#include "ccspec/matchers.h"

#include "helpers/fake_file.h"
#include "helpers/pack.h"
#include "helpers/allocateMemory.h"

using ccspec::core::describe;
using ccspec::core::before;
using ccspec::core::it;
using ccspec::expect;
using ccspec::matchers::eq;

extern "C" {
	#include "gd.h"
	#include "gd_internal.h"
}

namespace simple {

static uint8_t output[2048];
static gd_main_t main;
static gd_image_block_t image_block;

// reallocate memory for the expander
static void resetMemory() {
    // todo one source of truth
    image_block.output = output;
    image_block.unpack.expandCodes.output = output;
    image_block.unpack.expandCodes.outputLength = 0;
    image_block.unpack.expandCodes.outputCapacity = sizeof(output);
    image_block.unpack.expandCodes.string_table.memory = allocate();
}

auto image_data_block_spec =
describe("image data block", [] {
    before("all", [] {
        main.fread = ff_read;
    });
    describe("min code size 8", [] {
        describe("no subblock", [] {
            static uint8_t block[] = { 0x08, 0x00 };
            before("all", [] {
                FFILE(block);
                gd_image_block_read(&main, &image_block);
            });
            it("size", [] {
                expect(image_block.unpack.expandCodes.minumumCodeSize).to(eq(8));
            });
        });
        describe("simple subblock", [] {
            static uint8_t block[] = { 0x08, 0x04, 0x00, 0x01, 0x04, 0x04, 0x00 };
            before("all", [] {
                resetMemory();
                FFILE(block);
                gd_image_block_read(&main, &image_block);
            });
            it("size", [] {
                expect(image_block.unpack.expandCodes.minumumCodeSize).to(eq(8));
            });
        });
    });
    describe("min code size 2", [] {
        describe("simple subblock", [] {
            static uint8_t block[] = { 0x02, 0x02, 0x4c, 0x01, 0x00 };
            before("all", [] {
                resetMemory();
                FFILE(block);
                gd_image_block_read(&main, &image_block);
            });
            it("code size", [] {
                expect(image_block.unpack.expandCodes.minumumCodeSize).to(eq(2));
            });
        });
        // todo: validating
        describe("two sublocks", [] {
            before("all", [] {
                resetMemory();
                uint8_t lzwMinimumCodeSize = 2;
                uint16_t codeEnd = 5;

                // single subblock
                Pack p;
                p.reset();
                std::vector<uint8_t> unsplit;
                unsplit = p + 4 + 1 + 6 + 6 + Shift(4) + 2 + 9 + 9 + 7 + codeEnd;

                // split
                std::size_t split = unsplit.size() / 2;
                std::vector<uint8_t> head(unsplit.begin(), unsplit.begin() + split);
                std::vector<uint8_t> tail(unsplit.begin() + split, unsplit.end());

                // assemble block
                std::vector<uint8_t> block;
                block.emplace_back(lzwMinimumCodeSize);
                block.emplace_back(head.size());
                block.insert(block.end(), head.begin(), head.end());
                block.emplace_back(tail.size());
                block.insert(block.end(), tail.begin(), tail.end());
                block.emplace_back(0); // block end

                FFILEV(block);
                gd_image_block_read(&main, &image_block);
            });
            it("verify output", [&] {
                gd_expand_codes_t *expand = &image_block.unpack.expandCodes;
                vector<uint8_t> outputIndexes(expand->output, expand->output + expand->outputLength);
                expect(dump(outputIndexes)).to(eq("01,01,01,01,01,02,02,02,02,02,01,01,01,"));
            });
        });
    });
});

} // namespace simple
