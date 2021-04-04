# mcu-gif
A GIF decoder for embedded MCUs

The goals of this project are:

- mcu agnostic
- file system agnostic
- developed with TDD
- usable in C programming language

It would be nice to try additional languages. For MCUs, C is the most common.

## TDD with ccspec
Starting afresh with ccspec to try out comprehensible tests.
The GTest reports where just too bland.
The ccspec framework promises at least a basic rspec test report.

## Running the ccspec tests
Since this is a TDD project, we'll start by running the tests.
The steps are:
- update the Git submodule
- generate the make files with CMake
- run the tests

### Update the Git submodule
The ccspec git repository is linked as a submodule.
You'll need to update the git submodule with:

``git submodule update --init --recursive``

### Generate the make files with CMake
Run Cmake in the build directory with:

``cmake ..``

> There's a bug in ccspec. In submodules/ccspec/src/CMakeLists.txt,
> in the target_include_directories command,
> replace CMAKE_SOURCE_DIR with CMAKE_CURRENT_SOURCE_DIR.
> and add '../' before include.

### Run the tests
The following command will run the tests and display the test report:

``spec/start``

> Notice the nested structure of the test report.

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


## Debugging
Example on MacOS using lldb

In CXXFLAGS, add ``-g -O0``

Start the lldb debugger: ``lldb build/gd-test``

Launch a process to debug: ``process launch --environment GTEST_FILTER=DecodeSubBlock.simple``

Set a breakpoint: ``br set -f gd-test.cpp -l 148``

Run to breakpoint: ``r``

## Visual test
To visually verify the decoder, a small GUI is included. It can be run on the development host.

### SDL2 setup for Mac
Download the dmg from libsdl.org.
Copy SDL2.framework to either:
- the project root directory
- your home directory (such as ~/Library/)
- a global directory (such as /Library/Frameworks)

### Building and running the visual test
- cd to ``demo-sdl2``
- run ``make``
- run ``./main``

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

https://github.com/zhangsu/ccspec

https://github.com/zhangsu/ccspec-example

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

 
