# libstephen ![Build Status](https://travis-ci.org/brenns10/libstephen.svg?branch=master)

- Author: Stephen Brennan
- Version: Prerelease
- Homepage: https://github.com/brenns10/libstephen
- License: Revised BSD

## Description

libstephen is a C library that provides a number of basic tools for C
programming.  I primarily started it so I could implement some data structures
in C, and now it is the basis for some of my other C programming projects.

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

## Documentation

Technical documentation on the source code and functions is available as
documentation comments in source code.  This can be put into a more readable
form by running the command "make docs" in the project root directory.
Reference materials will be produced in the doc/ folder.  Please note that the
tools Doxygen and Graphviz are required to produce this documentation.

Alternatively, you can find the latest generated docs at the
[libstephen tool site](http://stephen-brennan.com/libstephen).

The GitHub Wiki has slightly more high-level, user friendly documentation.  It's
a good place to go to get a handle on how you should use the library.

## Test Coverage

This library uses its own unit testing framework to test itself.  Coverage
information is recorded by the program `gcov`, aggregated by the program `lcov`,
and converted into a very nice HTML file by `genhtml`.  This is very useful for
detecting what code needs tests.  To generate test coverage information, run the
following command. at the project root:

    $ ./coverage.sh

At which point you can open the file `cov/index.html` in a browser and view the
test coverage info.  Alternatively, you can find the latest test coverage
information, on the
[libstephen tool site](http://stephen-brennan.com/libstephen).

## Plans

For information on future plans for the library, see the [to-do list](TODO.md).

## License

Copyright (c) 2013-2015, Stephen Brennan.  All rights reserved.  See
[the license](LICENSE.txt) for details.
