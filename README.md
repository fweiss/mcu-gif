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

### SDL2 setup for Mac
Download the dmg
Copy the framework to the project root directory

## Running the unit tests
In the test directory, run ``make test``.

### Setting up the unit tests
Download the following:

- gtest
- fff test

You can put them anywhere on your file system.

Create a file ``local.mk`` in the test directory. This file will be included from the Makefile.
It must define the following variables:

- GTEST_HOME
- FFF_HOME


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
 