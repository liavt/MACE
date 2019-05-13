[![Build Status](https://travis-ci.org/liavt/MACE.svg?branch=master)](https://travis-ci.org/liavt/MACE)
[![Build status](https://ci.appveyor.com/api/projects/status/dovlvxm1xrg5bewc?svg=true)](https://ci.appveyor.com/project/liavt/mace)

# MACE
MACE stands for MACE is A C++ Engine, Modular Abstract Components Engine, Many Amazing Classes Everywhere, Many Acronyms Can Exist, or the medievel weapon. Choose whichever one you like best.

## What is in it?
It has many things:
* Hardware accelerated graphics library
* Subpixel text renderer using Freetype
* Audio player
* Serial communication
* DLL and process hanlding
* Vector and Matrix math library
* Macros to determine compilers and systems
* Module loader

You can implement your own modules to make MACE whatever you want!

## Compatibility
The graphics capabilities target hardware with OpenGL 3.3 compatibility

The following compilers are unit tested automatically. The `master` branch will always have a release that passes all unit tests.
* G++ 5 and 6 on Linux
* Clang 4.0 and 3.5 on Linux
* Clang 3.5 on OSX
* Latest and second latest version of VS 2015 and 2017 on Windows

MACE requires a C++ 11 compliant compiler

## Installation

For install instructions, consult INSTALLATION.md in the root folder.

## Documentation

Documentation is located [here.](https://liavt.github.io/MACE/html/index.html)

To generate documentation on your own, download Doxygen. Then, run Doxygen with `Doxyfile` located in `docs` branch.

A new folder called *out* should have been created. Subfolders contain the different documentation types, like html or xml.

## Important Notes
* You should never delete any pointer given to you by MACE and MACE will never delete any pointer you give it unless explicitely specified.
* MACE has varying levels on abstraciton, and gives you the power to change most of these levels. You can completely change how MACE works without touching the source code. Doing this may be dangerous if you don't know what you are doing
