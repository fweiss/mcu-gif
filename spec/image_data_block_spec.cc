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
    describe("no subblock", []{
        // todo check errors
        describe("minimum code size 2", [] {
            before("all", [] {
                static uint8_t block[] = { 0x02, 0x00 };
                FFILE(block);
                gd_image_block_read(&main, &image_block);
            });
            it("no ouput", []{
                expect(image_block.unpack.expandCodes.outputLength).to(eq(0));
            });
            it("minimum code size", [] {
                expect(image_block.unpack.expandCodes.minumumCodeSize).to(eq(2));
            });
            it("entries", [] {
                expect(image_block.unpack.expandCodes.string_table.entries_length).to(eq(0));
            });
        });
        describe("minimum code size 8", [] {
            before("all", [] {
                static uint8_t block[] = { 0x08, 0x00 };
                FFILE(block);
                gd_image_block_read(&main, &image_block);
            });
            it("minimum code size", [] {
                expect(image_block.unpack.expandCodes.minumumCodeSize).to(eq(8));
            });
        });
    });
    describe("simple subblock", [] {
        describe("min code size 2", [] {
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
        describe("min code size 8", [] {
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
    describe("10x10 reference block", [&] {
        static Pack p;
        static uint8_t lzwMinimumCodeSize = 2;
        static uint8_t blockEnd = 0;

        before("all", [&] {
            p.reset();
            // this works as one block of length 4
            vector<uint8_t> zinger = p + 4 + 1 + 6 + 6 + Shift(4) + 2 + 9 + 9 + 7 + 5;

            // and spilt in blocks
            const size_t split = 2;
            vector<uint8_t> split0(zinger.begin(), zinger.begin() + split);
            vector<uint8_t> split1(zinger.begin() + split, zinger.end());

            // note block size is 0x01-0xff, but vector size is sixe_t
            // consequently static_cast<uint8_t> is needed
            vector<uint8_t> block;
            block.emplace_back(lzwMinimumCodeSize); // lzw minumum code size

            block.emplace_back(static_cast<uint8_t>(split0.size())); // first subblock length
            block.insert(block.end(), split0.begin(), split0.end());

            block.emplace_back(static_cast<uint8_t>(split1.size())); // second subblock length
            block.insert(block.end(), split1.begin(), split1.end());

            block.emplace_back(blockEnd); // block end

            FFILEV(block);

            gd_image_block_read(&main, &image_block);
        });
        it("verify output", [&] {
            gd_expand_codes_t &expand = image_block.unpack.expandCodes;
            // vector<uint8_t> outputIndexes(image_block.output, image_block.output + image_block.outputLength);
            vector<uint8_t> outputIndexes(expand.output, expand.output + expand.outputLength);
            expect(dump(outputIndexes)).to(eq("01,01,01,01,01,02,02,02,02,02,01,01,01,"));
        });
    });
    // todo: validating
    describe("two sublocks", [] {
        gd_expand_codes_t &expand = image_block.unpack.expandCodes;
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
        it("output length", [&] {
            // not verified
            expect(expand.outputLength).to(eq(13u));
        });
        it("has one index", [&] {
            expect((int)expand.output[0]).to(eq((gd_index_t)1));
        });
        it("verify output", [&] {
            gd_expand_codes_t *expand = &image_block.unpack.expandCodes;
            vector<uint8_t> outputIndexes(expand->output, expand->output + expand->outputLength);
            expect(dump(outputIndexes)).to(eq("01,01,01,01,01,02,02,02,02,02,01,01,01,"));
        });
    });
});

} // namespace simple
