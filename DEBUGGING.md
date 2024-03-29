# Debugging
Notes and instructions for debugging:
- both spec tests and demo-sdl
- both MacOS and Windows

## MacOS and lldb
Currently some info in the main README.md

## Windows
Modified several CMakeList.txt file for use with MSVC Command line tools

## Image
On MacOS, use Digital Color Meter to compare colors displayed in Preview
with those displayed in demo-sdl2.

## 128px-Dancing.gif
First pixel should be rgb(58,86,148) but was rgb(0,0,153)
First pixel index is 3 should be 43 rgb(51,85,153) as per GIMP
51 = 0x33 = 0011 0011
43 = 0x2b = 0010 1011

suspect gd_image_subblock_decode

0(6): gif header
6(7) logical screen descriptor
13(768): global color table
### graphic control extension
781/30D(8): 21 
### first image descriptor
789/315(10): 2C
0000 left
0000 right
8000 width (LE)
8000 height (LE)
00

### first image data block
799/31f():
31f: 08 lzw minimum code size (clear = 256/0x00)
320: ff sub block size (420, 520, 620, ... 2620: 182/b6, 26d6: 00)
321; 00 5708 4482 a444 8981 0413 2a34 c850
0000 0000 0101 0111 0000 1000 0100 0100 1000 0010 1010 0100 0100 0100 1000 1001 1000 0001
aaaa aaaa abbb bbbc cccc cc
LE:
0000 0000 1010 1110 0000 0001 0010 0010 0001 0100 0101 0010 0010 0010 0001 1001 0001 1000
aaaa aaaa abbb bbbb bb
a=1 0000 0000 256
b=0 0011 1010  58

21c1 2448 9248 44d2 c460 4487 0a57 6024
28b0 c9c6 841e 4b7c 0459 8284 4912 2b9a

## Access exception in
in expand_state_spec, near line 60
this goes away if ``gd_string_table_init(&expand.string_table, 2);``
is inserted in expand_state_spec.cc in the before each
but then other errors occur

## BeTruthy

``error LNK2001: unresolved external symbol "class ccspec::matchers::BeTruthy const & const ccspec::matchers::be_truthy" (?be_truthy@matchers@ccspec@@3ABVBeTruthy@12@B)``

try commenting out the reference in ``10x10_red_blue_white_cc``

> this problem showed up later in the ccspec project

then run ``spec\start

can't start because ccspec.dll is missing

copy spec\build\ccspec to spec

then runs, output

but runtime error variable 'succeeded' is used but not initialized

maybe /MD vs /MT - nah, that's for multi-thread

decide to try cmake install. this works by putting the start and dll in the build/bin folder
was avoiding it because it seemed to add a step that wasn't needed on the OSX

getting back to the linker problem

in ``be_truthy.h`` we got ``extern const BeTruthy& be_truthy;``.

Recqapping the bug: ``10x10_red_blue_white.cc.obj : error LNK2001: unresolved external symbol "class ccspec::matchers::BeTruthy const & const ccspec::matchers::be_truthy" (?be_truthy@matchers@ccspec@@3ABVBeTruthy@12@B)``

Which it riggered by "10x10 red" around line 55, It's attempting
to pass the global in be_truthy to ``expect().to(be_truthy)``.

So the linker is looking for that montsrsity above, basicallY
"const reference to const BeTruthy"

Maybe because it's in the .h file?

Let's look at the fucntion reference. 

Bug appear to be in betruthy.cc, where the initializer ``extern const BeTruthy& be_truthy;``
is missing the inner const.

## Unable to open 'deque'
In VSC debugger, launch spec\ccspec.

> this turned out to be old code in submodules/ccspec/build from an older version
> of MSVC 2019

```
Unable to open 'deque': Unable to read file 'c:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Tools\MSVC\14.28.29910\include\deque'
```

1. how come it's looking for deque?
2. how come it's looking in 14.28.29910 instead of 14.29.30037?
3. and cmake idenfies compiler 19.29.30040.0?

#1 probably because example_group.cc has:
using std::stack

Visual Studio Installer
Visual Studio Build Tools 2019

Try under Individual Components > Compiler, build tools, and runtimes
1. check C++/CLI support for v142 build tools (Latest)
2. check C++/CLI support for v142 build tools (1428-16.9)

#2 added the missing directory, but not include

3. Let's uninstall and reinstall

After uninstall, empty directory
c:\Program Files (x86)\Microsoft Visual Studio\2019

Visual Studio Installer
Available
Visual Studio Enterprise 2019
Install
Individual Components
Compiler, build tools, and runtimes
C++ Clang-cl for v142 build tools (x64/x86)
C++ CMake tools for Windows
C++/CLI support for v142 build tools (Latest)
No workloads
No signin

after install
c:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise

cmake is not recognized in developer command prompt

OK, let's start over
=========================

Visual Studio Installer
Available
Visual Studio Professional 2019
Workloads
Desktop development with C++
Don't signin
Developer PowerShell

Either Community, Professional, or Enterprise will work for CLI tools.

try
SDKs, libraries, and frameworks
C++ v14.29 (16.8) ATL for v142 build tools (x86 & x64)

Enterprise didn't work, added directory level

