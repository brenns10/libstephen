# libstephen ![Build Status](https://travis-ci.org/brenns10/libstephen.svg?branch=master)

- Author: [Stephen Brennan](http://brennan.io)
- Version: Prerelease
- Homepage: https://github.com/brenns10/libstephen
- Tool site: http://brennan.io/libstephen
- License: Revised BSD

## Description

libstephen is a C library that provides a number of basic tools for C
programming.  I primarily started it so I could implement some data structures
in C, and now it is the basis for some of my other C programming projects.  Its
features include:

### Data Structures

- Linked list (ll)
- Array list (al)
- Hash table (ht)
- Bit field (bf)
- Character buffer (similar to a string builder in Java)

### Tools

- Unit testing framework (su)
- Command line argument parser
- Logging framework
- Regular expressions

## Build

This project is built with CMake. Typical usage involves creating a build
directory (`mkdir build`), entering it (`cd build`), running CMake on the source
directory (`cmake ..`), and finally compiling with make (`make`).

The more compact way is to run:

    cmake -Bbuild -H. && cmake --build build

Even though I use CMake, I still have a Makefile to automate CMake tasks, so you
can do `make release` and `make debug` to automatically CMake and build those
configurations, and `make test` to CMake, build, and run some tests in the debug
configuration.

## Documentation

If you want documentation on this library, well you're in luck!  One of the
things I have experimented with is automatically producing and publishing
up-to-date documentation.  You can find documentation up-to-date with master in
the [documentation section][doc] of the tool site.  This includes both
tutorial/commentary style documentation, as well as function-level documentation
generated from the code.

You can generate this documentation yourself by running the command `make doc`
in the project root directory. This requires CMake, Doxygen, Python, Sphinx, and
the sphinx-rtd-theme to be installed.

## Test Coverage

This library uses its own unit testing framework to test itself.  Coverage
information is recorded by the program `gcov`, aggregated by the program `lcov`,
and converted into a very nice HTML file by `genhtml`.  This is very useful for
detecting what code needs tests.  To generate test coverage information, run the
following command. at the project root:

    $ make cov

At which point you can open the file `cov/index.html` in a browser and view the
test coverage info.  Alternatively, you can find the latest test coverage
information on the [libstephen tool site][].

## Plans

My most up-to-date ideas are now maintained on the [GitHub Issues][]

## License

Copyright (c) 2013-2016, Stephen Brennan.  All rights reserved.  Released under
the Revised BSD license.  See [LICENSE.txt][] for details.

[libstephen tool site]: http://brennan.io/libstephen
[doc]: http://brennan.io/libstephen/doc/
[GitHub Issues]: https://github.com/brenns10/libstephen/issues
[GitHub Wiki]: https://github.com/brenns10/libstephen/wiki
[LICENSE.txt]: LICENSE.txt
