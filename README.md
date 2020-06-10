# mcu-gif

A GIF decoder for MCUs


The goals of this project are:

- mcu agnostic
- file system agnostic
- developed with TDD
- developed in C programming language

It would be nice to try additional languages. For MCUs, C is the most common.

> The test framework, gtest, is C++

## Visual testing
The visually verify the decoder, a small GUI is included. It can be run on the development host.

Several portable GUI frameworks were investigated:

- GTK - to heavy weight
- QT 
- EasyBMP 
- SDL2 - well supported, OS X binaries
- graphics.h - too old
- openGL - may be poorly supported on OS X

## Debugging
Example on MacOS using lldb

In CXXFLAGS, add ``-g -O0``

Start the lldb debugger: ``lldb build/gd-test``

Launch a process to debug: ``process launch --environment GTEST_FILTER=DecodeSubBlock.simple``

Set a breakpoint: ``br set -f gd-test.cpp -l 148``

Run to breakpoint: ``r``

### SDL2 setup for Mac
Download the dmg
Copy the framework to the project root directory

## Running the unit tests
In the test directory, run ``make test``.

## CCSpec
I think RSpec is a good model for writing tests. There has been some effort to provide this for C/C++.

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
 