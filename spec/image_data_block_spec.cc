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

extern "C" {
	#include "gd.h"
	#include "gd_internal.h"
}

namespace simple {

// fixme share
// for comparing vectors, until ccspec has a diff reporter
static std::string dump(std::vector<uint8_t> p) {
    std::string o;
    for (auto b : p) {
        char buf[3];
        snprintf(buf, sizeof(buf), "%02X", b);
        o += buf;
        o += ",";
    }
    return o.c_str();
}

const size_t outputSize = 1024;  // fixme max output size for a sub block?

typedef std::vector<uint8_t> code_stream_t;

auto image_data_block_spec =
describe("image data block", [] {

    static gd_index_t output[outputSize];

    static gd_main_t main;
    main.fread = ff_read;
    static gd_image_block_t image_block;
    image_block.output = output;

    before("each", [] {
        // N.B. 'output' must be the array, not a pointer
        memset(output, 0, sizeof(output));
    });

    describe("empty", [&] {
        before("each", [&] {
            static uint8_t input[] = { 0x02, 0x00 };
            FFILE(input);
            gd_image_block_read(&main, &image_block);
        });
        it("output length", [&] {
            expect(image_block.outputLength).to(eq(0));
        });
    });

    describe("minimum code size 2", [&] {
        before("each", [&] {
            static uint8_t input[] = { 0x02, 0x00 };
            FFILE(input);
            gd_image_block_read(&main, &image_block);
        });
        it("value", [&] {
            expect(image_block.minumumCodeSize).to(eq(2));
        });
    });

    describe("minimum code size 8", [&] {
        before("each", [&] {
            static uint8_t input[] = { 0x08, 0x00 };
            FFILE(input);
            gd_image_block_read(&main, &image_block);
        });
        it("value", [&] {
            expect(image_block.minumumCodeSize).to(eq(8));
        });
    });

    describe("one sub block", [&] {

        describe("basic", [&] {
            before("each", [&] {
                static uint8_t input[] = { 0x02, 0x02, 0x4C, 0x01, 0x00 };
                FFILE(input);
                gd_image_block_read(&main, &image_block);
            });
            it("output length", [&] {
                expect(image_block.outputLength).to(eq(1));
            });
            it("has one index", [&] {
                expect((int)image_block.output[0]).to(eq(1));
            });
        });

        // failing because string min Code size isn't recognized
        describe("largest min code size 8", [&] {
            before("each", [&] {
                // static uint8_t input[] = { 0x08, 0x03, 0x00, 0x07, 0x00 };
                static uint8_t input[] = { 0x08, 0x04, 0x00, 0x07, 0x04, 0x04, 0x00 };
                FFILE(input);
                gd_image_block_read(&main, &image_block);
            });
            it("output length", [&] {
                expect(image_block.outputLength).to(eq(1));
            });
            it("index[0]", [&] {
                expect((uint16_t)image_block.output[0]).to(eq(3));
            });
        });
    });
    describe("10x10 reference block", [&] {
        static Pack p;

        before("each", [&] {
            p.reset();
            // this works as one block of length 4
            vector<uint8_t> zinger = p + 4 + 1 + 6 + 6 + Shift(4) + 2 + 9 + 9 + 7 + 5;

            // and spilt in blocks
            const size_t split = 2;
            vector<uint8_t> split0(zinger.begin(), zinger.begin() + split);
            vector<uint8_t> split1(zinger.begin() + split, zinger.end());

            // build the image block
            // vector<uint8_t> block;
            // block.emplace_back(2);
            // block.emplace_back(9);
            // block.insert(block.end(), zinger.begin(), zinger.end());
            // block.emplace_back(0); // block end

            vector<uint8_t> block;
            block.emplace_back(2);
            block.emplace_back(split0.size());
            block.insert(block.end(), split0.begin(), split0.end());
            block.emplace_back(split1.size());
            block.insert(block.end(), split1.begin(), split1.end());
            block.emplace_back(0); // block end


            FFILEV(block);

            gd_image_block_read(&main, &image_block);
        });
        it("outputs", [&] {
            vector<uint8_t> output(image_block.output, image_block.output + image_block.outputLength);
            expect(dump(output)).to(eq("01,01,01,01,01,02,02,02,02,02,01,01,01,"));
        });
    });
    describe("two subblocks", [&] {
        static Pack p;
        static std::vector<uint8_t> packed;

        before("each", [&] {
            p.reset();
        });

        before("each", [&] {
            // static uint8_t input[] = { 0x02, 0x02, 0x4C, 0x01, 0x00 };
            // FFILE(input);

            // vector<uint8_t> file = p + 4 + 0 + 1 + 5; 
            // FFILE(file);
            vector<uint8_t> subblocks[2];
            p.reset();
            subblocks[0] = p + 4 + 1 + 6;
            p.reset();
            subblocks[1] = p + 6 + Shift(4) + 2 + 9 + 9 + 7 + 5;

            vector<uint8_t> block;
            block.emplace_back(2); // minimum block size
            block.emplace_back(2); // subblock length
            block.insert(block.end(), subblocks[0].begin(), subblocks[0].end());
            block.emplace_back(3);
            block.insert(block.end(), subblocks[1].begin(), subblocks[1].end());
            block.emplace_back(0x00); // block end

            // alternate with just one block
            p.reset();
            vector<uint8_t> block2;
            block2.emplace_back(2);
            block2.emplace_back(9);
            vector<uint8_t> zinger = p + 4 + 1 + 6 + 6 + Shift(4) + 2 + 9 + 9 + 7 + 5;
            block2.insert(block2.end(), zinger.begin(), zinger.end());
            block2.emplace_back(0); // block end

            FFILEV(block2);

            // this should be packed 8C,AD,02,

            gd_image_block_read(&main, &image_block);
        });
        it("output length", [&] {
            // not verified
            expect(image_block.outputLength).to(eq(13));
        });
        it("has one index", [&] {
            expect((int)image_block.output[0]).to(eq(1));
        });
        it("output", [&] {
            vector<uint8_t> output(image_block.output, image_block.output + image_block.outputLength);
            // not verified
            // but equivalent to p + 4 + 1 + 6 + 6 + 2 + 5
            expect(dump(output)).to(eq("01,01,01,01,01,02,02,02,02,02,01,01,01,"));
        });
    });
});

} // namespace simple
