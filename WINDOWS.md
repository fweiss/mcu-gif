# Windows
Guide for building on Windows

## Overview
There are several toolchain options on Windows.
I chose the bare-bones Visual Studio 2019 command line tools.

- Windows 10
- Visual Studio Professional 2019
- Desktop (CLI) tools

## Build and run tests
-  open a Developer Command Prompt for Visual Studio 2019
- ``cd build``
- ``cmake -G "NMake Makefiles" ..``
- ``nmake install``
- ``bin\start.exe``

You should see a tree of the test specs.

> Use Developer PowerShell or Developer Command prompt at your option.

> TODO Should rename start to test.

## Install toolchain
Download and install "Visual Studio Installer".

Visual Studio Installer
- Available
- Visual Studio Professional 2019
- Workloads
- Desktop development with C++
- Don't signin

You can now open "Developer PowerShell for VS 2019"

> I think this included CMake tailored for MSVC.

## Debugging tips
Some tips on debugging

### Progasm exit code
To see the program's exit code, enter ``$LastExitCode``

The output will be decimal. It's usually useful to convert to hex.

> Example: C0000135 missing dll
