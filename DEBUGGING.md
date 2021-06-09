## Windows
Modified several CMakeList.txt file for use with MSVC Command line tools

nmake

``error LNK2001: unresolved external symbol "class ccspec::matchers::BeTruthy const & const ccspec::matchers::be_truthy" (?be_truthy@matchers@ccspec@@3ABVBeTruthy@12@B)``

try commenting out the reference in ``10x10_red_blue_white_cc``

then run ``spec\start

can't start because ccspec.dll is missing

copy spec\build\ccspec to spec

then runs, output

but runtime error variable 'succeeded' is used but not initialized

maybe /MD vs /MT - nah, that's for multi-thread

decide to try cmake install. this works by putting the start and dll in the build/bin folder
was avoiding it because it seemed to add a step that wasn't needed on the OSX
