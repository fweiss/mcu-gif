#include <stdlib.h>
#include <iostream>

using std::cout;

#include "ccspec/core.h"

#include "ccspec/expectation.h"
#include "ccspec/matchers.h"


using ccspec::core::formatters::DocumentationFormatter;
using ccspec::core::Reporter;
using ccspec::core::ExampleGroup;

namespace simple {
    extern ExampleGroup* spec_10x10_red_blue_white;
    extern ExampleGroup* image_data_block_spec;
    extern ExampleGroup* helpers_spec;
    extern ExampleGroup* image_subblock_spec;
    extern ExampleGroup* image_expand_spec;
    extern ExampleGroup* string_table_spec;
    extern ExampleGroup* expand_state_spec;
    extern ExampleGroup* read_block_spec;
    extern ExampleGroup* read_block_error_spec;
    extern ExampleGroup* image_decompress_spec;
}

int main() {
    ExampleGroup* example_groups[] = {

        simple::helpers_spec,

        // bottom up order
        simple::string_table_spec,
        simple::expand_state_spec,
        simple::image_expand_spec,
        simple::image_subblock_spec,
        simple::image_data_block_spec,
        simple::spec_10x10_red_blue_white,
        simple::read_block_spec,
        simple::read_block_error_spec,

        simple::image_decompress_spec,
    };

    DocumentationFormatter formatter(cout);
    Reporter reporter(&formatter);

    bool succeeded = true;
    for (auto example_group : example_groups) {
        succeeded = example_group->run(reporter) && succeeded;
        delete example_group;
    }

    return !succeeded;
}
