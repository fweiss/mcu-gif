# mcu-gif

A GIF decoder for MCUs


The goals of this project are:

- mcu agnostic
- file system agnostic
- developed with TDD
- developed in C programming language

It would be nice to try additional languages. For MCUs, C is the most common.

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

## Links and references

Really easy and comprehensive guide to decoding GIFs:
 
https://commandlinefanatic.com/cgi-bin/showarticle.cgi?article=art011

More detailed description bit-by-bit

http://www.matthewflickinger.com/lab/whatsinagif/bits_and_bytes.asp

https://www.geeksforgeeks.org/sdl-library-in-c-c-with-examples/

https://lazyfoo.net/tutorials/SDL/01_hello_SDL/mac/index.php

https://www.willusher.io/sdl2%20tutorials/2013/08/17/lesson-1-hello-world
 