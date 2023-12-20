# mcu-gif
A GIF decoder for embedded MCUs

The goals of this project are:

- MCU agnostic
- file system agnostic
- developed with TDD
- usable in C programming language

It would be nice to try additional languages. For MCUs, C is the most common.

## TDD with ccspec
Initally GTest was used to start development.
I felt the GTest reports where just too bland.
They were "flat" and had a bunch of noisy formatting
I was used to "spec" tests with Jasmine.
I found the ccspec framework which promised at least a basic rspec test report.
So I started afresh with ccspec to try out [comprehensible tests](https://fweiss.github.io/test-driven/test-fluency-and-comprehension).

## Running the ccspec tests
Since this is a TDD project, we'll start by running the tests.
The steps are:
- update the Git submodule
- build and run the tests

### Update the Git submodule
The ccspec git repository is linked as a submodule.
You'll need to update the git submodule with:

``git submodule update --init --recursive``

### Build and run the tests
This is a CMake project, so it follows that workflow.

> See WINDOWS.md for a guide specific to Windows.
> The following is for MacOS.

Example workflow:

```
mkdir build
cd build
cmake ..
make install
spec/start
```

> Notice the nested structure of the test report.

## Debugging
Example on MacOS using lldb

> Make sure executable includes debugging info: ``cmake -DCMAKE_BUILD_TYPE=Debug ..``

Start the lldb debugger: ``lldb build/gd-test``

Launch a process to debug: ``process launch --environment GTEST_FILTER=DecodeSubBlock.simple``

Handy commands:
- add debug info to binaries: ``cmake -DCMAKE_BUILD_TYPE=Debug ..``
- Set a breakpoint: ``br set -f gd-test.cpp -l 148``
- Run to breakpoint: ``r``
- step over: ``n``
- stack trace: ``thread backtrace``
- local variables: ``frame variable``
- array: ``parray 100 pixels``

> Hex dump of gif file. Use vi with command ``:%!xxd``
>  Or use ``xxd``

## Read image data call graph
gd_read_image_data(&main, pixels, imd.image_size)
  gd_image_block_read(main, &image_block)
    gd_expand_codes_init(&image_block->expand_codes, image_block->output)
    gd_image_subblock_decode(image_block, subblock, subblockSize) // unpack block to codes
      gd_image_expand_code() // decompress code to indexes
        gd_string_table_init()
        gd_string_table_at()
        gd_string_table_add()

### gd_read_image_data()
- API function
- given gd_main_t with fp and fread
- given output buffer and capacity
- init image_block_t with output buffer
- delegate to gd_image_block_read()

### gd_image_block_read()
- given gd_main_t
- given gd_image_block_t
- init gd_expand_codes_t member of gd_image_block_t, copy data
- read minimum code size
- init image_block and image_block.expand_codes with code size info
- read subblock size
- allocate subblock buffer on stack
- read image subblock
- decode sub block via gd_image_subblock_decode, using image_block

### gd_image_subblock_unpack()
- given gd_image_block
- given a subblock buffer and size
- unpacks it to a stream of codes
- passes the stream of unpacked codes to gd_image_expand_code()
- needs to know code bits/code mask
- needs to know shift out
- needs to know when code size changes

> This was inverted to avoid a callback or state machine.
> but that may be what makes the structures wonky.
> Its function is unpack not decode.

### gd_image_code_expand()
- given gd_codes_expand_t with outout and capacity
- given gd_codes_expand_t with gd_string_table_t
- given gd_codes_expand_t with codeSize, clearCode, priorString, compressStatus
- given an unpacked code
- adds indices to the output
- builds the code table
- needs to know string table
- needs to know string table size
- needs to know clear code/end of information code

> This could be a state machine that shifts out codes from the byte stream.

## Visual test
To visually verify the decoder, a small GUI is included. It can be run on the development host.

### SDL2 setup for Mac
Download the dmg from libsdl.org.
Copy SDL2.framework to either:
- the project root directory
- your home directory (such as ~/Library/)
- a global directory (such as /Library/Frameworks)

There are some permissions that need to be enabled on MacOS.
First is in security, second is at launch.

> Use global - there's some absolute path wired into the library.

> Use version 2.0.22
> Latest has a problem with the Headers needing SDL2 subdirectory.

### Building and running the visual test
Example workflow:
```
cd demo-sdl2
make
./main
```

You should see a window open, displaying a GIF.

> This subproject has not been converted to CMake

### Alternative GUI visualizers
Several portable GUI frameworks were investigated:

- GTK - too heavy weight
- QT - a whole ecosystem
- EasyBMP 
- SDL2 - well supported, OS X binaries
- graphics.h - too old
- openGL - may be poorly supported on OS X

## CCSpec
RSpec is a great model for developing software using TDD.
I think ccpec provide a solid implementation for C/C++.

https://github.com/fweiss/ccspec

https://github.com/fweiss/ccspec-example

## Unit testing with gtest (deprecated)
This project was originally developed with TDD using gtest.

> The gtest example code is in the ``gtest`` branch.

## Eclipse notes
Finally got <vector> initializer list working. 
The trick is that the LLVM library include file is "vector" not "vector.h"
consequenlty the default indexer settings to not properly index it.

Project > Propoerties > C/C++ General > Indexer:
- Enable project-specific settings
- Index all variants of specific headers: enter "vector"

https://wiki.eclipse.org/CDT/User/NewIn83#Toolchains

## Bugs
```
string_table_spec.cc
NMAKE : fatal error U1073: don't know how to make 'spec\ccspec\ccspec.lib'
Stop.
NMAKE : fatal error U1077: '"C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Tools\MSVC\14.29.30037\bin\HostX86\x86\nmake.exe"' : return code '0x2'
```

## Links and references

Really easy and comprehensive guide to decoding GIFs:
 
https://commandlinefanatic.com/cgi-bin/showarticle.cgi?article=art011

More detailed description bit-by-bit

http://www.matthewflickinger.com/lab/whatsinagif/bits_and_bytes.asp

Best LZW for GIF description with examples

https://www.eecis.udel.edu/~amer/CISC651/lzw.and.gif.explained.html

https://www.geeksforgeeks.org/sdl-library-in-c-c-with-examples/

https://lazyfoo.net/tutorials/SDL/01_hello_SDL/mac/index.php

https://www.willusher.io/sdl2%20tutorials/2013/08/17/lesson-1-hello-world

Some good insights into TDD applied to GIF decoding:

http://debuggable.com/posts/test-driven-development-in-real-world-apps:480f4dd5-514c-45f2-b3dc-4a16cbdd56cb

[The official W3C GIF89a specification](https://www.w3.org/Graphics/GIF/spec-gif89a.txt)

A good summary of the GIF file format:

http://netghost.narod.ru/gff/graphics/summary/gif.htm
