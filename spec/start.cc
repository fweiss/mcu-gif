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
    extern ExampleGroup* image_block_spec;
    extern ExampleGroup* helpers_spec;
    extern ExampleGroup* image_subblock_spec;
}

int main() {
    ExampleGroup* example_groups[] = {
        simple::helpers_spec,
        simple::image_subblock_spec,
        simple::image_block_spec,
        simple::spec_10x10_red_blue_white,
    };

    DocumentationFormatter formatter(cout);
    Reporter reporter(&formatter);

    bool succeeded = true;
    for (auto example_group : example_groups) {
        bool succeeded = example_group->run(reporter) && succeeded;
        delete example_group;
    }

    return !succeeded;
}
