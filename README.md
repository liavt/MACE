[![Build Status](https://travis-ci.org/liavt/MACE.svg?branch=master)](https://travis-ci.org/liavt/MACE)
[![Build status](https://ci.appveyor.com/api/projects/status/dovlvxm1xrg5bewc/branch/master?svg=true)](https://ci.appveyor.com/project/liavt/mace/branch/master)

# MACE
MACE stands for MACE is A C++ Engine, Modular Abstract Components Engine, Many Amazing Classes Everywhere, Many Acronyms Can Exist, or the medievel weapon. Choose whichever one you like best.

## Goals

MACE aims to take advantage of the Entity-Component-System (ECS) model to create a highly configurable and modular graphics engine.

MACE will utilize hardware acceleration where possible in order to be as speedy as possible

MACE will work across all 3 major platforms (Windows, OSX, and Linux) to create a unified experience

In order to keep compile times low and binaries small, external dependencies will be minimized and highly specialized.

## Features

* Hardware accelerated graphics library using ECS (Entity-Component-System)
* Subpixel text renderer using Freetype
* Audio player
* Serial communication
* DLL and process hanlding
* Vector and Matrix math library
* Macros to determine compilers and systems
* Module loader

You can implement your own modules to make MACE whatever you want!

## Compatibility

The graphics capabilities target hardware with OpenGL 3.3 compatibility.

The following compilers are unit tested automatically. The `master` branch will always have a release that passes all unit tests.
* G++ 5 and 6 on Linux
* Clang 4.0 and 3.5 on Linux
* Clang 3.5 on OSX
* Latest and second latest version of VS 2015 and 2017 on Windows

MACE requires a C++ 11 compliant compiler.

## Installation

* Download the MACE source. 
	* This can be done via Git: Type `git clone https://github.com/liavt/mace.git` in terminal. On Windows, you will need MingW or a similar Unix emulator
	* You can also use  the Git GUI if you don't like command  line.
* Download CMake
	* On Windows systems, make sure to add it to your Path.
* Create a new folder called build in the MACE source directory
* Open the terminal.
	* On Windows systems, open the standard command prompt.
* Type `cd <path to where you downloaded the MACE source>`
	* On Windows, you may need to change to the drive via `cd \d  <drive letter>`
* Type `cd build`
* Finally, type `cmake ..`
	* MACE's dependencies will automatically be downloaded and installed in the build directory.
	* Downloading dependencies will require an internet connection.
	* To disable the downloading of dependencies, disable the `MACE_DOWNLOAD_DEPENDENCIES` option in the CMake configuration. Missing dependencies will cause a CMake error.
* If you are on Windows, a Visual Studio solution file should have been generated. Open it to find all of the source code linked and ready!
	* To build MACE, simply use Visual Studio to build the ALL-BUILD project
* If you are on OSX, you will have a choice of generating a Makefile or an XCode project
* If you are on Linux,  you will have a Makefile generated for you. Simply type `make <path to makefile>` to compile it via GCC

## Unit tests

Unit tests are located in the `tests` subdirectory, and use the [Catch2](https://github.com/catchorg/Catch2) testing library.

To build unit tests, enable the `MACE_TESTS` option in your CMake configuration.

Build the `MACE-Testing` target and an executable of the same name will be generated that will run the unit tests.

## Demos

The repo features a collection of single-file demos located in the `demos` subdirectory.

These are intended not only for educational purposes but also as high-level tests of MACE features.

To build demos, enables the `MACE_DEMOS` option in your CMake configuration.

A built target corresponding to each demo will be generated in the output Makefile.

## Documentation

Documentation is located [here.](https://liavt.github.io/MACE/html/index.html)

To generate documentation on your own, enable the `MACE_DOCS` option in your CMake configuration.

Then build the `MACE_DOCS` target and the documenation will be located in the `docs` subdirectory under your build folder.

## Important Notes
* You should never delete any pointer given to you by MACE and MACE will never delete any pointer you give it unless explicitely specified.
* MACE has varying levels on abstracition, and gives you the power to change most of these levels. You can completely change how MACE works without touching the source code. Doing this may be dangerous if you don't know what you are doing
