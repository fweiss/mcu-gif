# Windows
Guide for building and running on Windows

## Overview
There are several toolchain options for Windows.
I chose the bare-bones Visual Studio 2019 command line tools.

- Windows 10
- Visual Studio Professional 2019
- Desktop (CLI) tools

> See Install toolchain section to get started.

## Build and run tests
- open a Developer Command Prompt for Visual Studio 2019
- navigate to this project directory
- ``cd build``
- ``cmake -G "NMake Makefiles" ..``
- ``nmake install``
- ``bin\start``

You should see the test report as a tree of the test specs.

> Use Developer PowerShell or Developer Command prompt at your option.

> TODO Should rename start to test.

## Build and run demo-sdl2
This will run a SDL2 window that decodes and displays a gif file.
- cd ``demo-sdl2\build``
- run ``cmake -G "NMake Makefiles2" ..``
- cd ..
- run ``build\demo-sdl2.exe``

Setup the SDL2 toolchain as follows:
- go to https://github.com/libsdl-org/SDL/releases
- navigate to version 2.2.8
- download SDL2-devel-2.2.8-VC.zip
- unzip
- move it to a particular folder, such as ``\Libraries``
- before running cmake, set that folder
- before running demo-sdl2, add the folder plus ``\lib\x86`` to the path

## Install toolchain
Download and install "Visual Studio Installer".

Visual Studio Installer
- Select Available tab
- Visual Studio Professional 2019
- Workloads
- Desktop development with C++
- Don't signin

You can now open "Developer PowerShell for VS 2019"

> I think this included CMake tailored for MSVC.

## Debugging tips
Some tips on debugging

### Program exit code
To see the program's exit code, enter ``$LastExitCode``

The output will be decimal. It's usually useful to convert to hex.

> Example: C0000135 missing dll

## Links and references

### Microsoft Visual Studio command line tools
https://docs.microsoft.com/en-us/cpp/build/building-on-the-command-line?view=msvc-160
