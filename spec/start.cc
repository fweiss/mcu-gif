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
	extern ExampleGroup* addition_spec;
}

int main() {
	ExampleGroup* example_groups[] = {
		simple::addition_spec,
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
