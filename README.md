# libstephen ![Build Status](https://travis-ci.org/brenns10/libstephen.svg?branch=master)

- Author: [Stephen Brennan](http://stephen-brennan.com)
- Version: Prerelease
- Homepage: https://github.com/brenns10/libstephen
- Tool site: http://stephen-brennan.com/libstephen
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

## Documentation

The [GitHub Wiki][] is currently the best source for user friendly
documentation.  It's a good place to go to get a handle on how you should use
the library.

Documentation on individual functions is provided inline in Doxygen style.  This
is used to generate an API documentation site.  This is automatically done by
Travis and published at the [libstephen tool site][].

You can generate documentation yourself by running the command "make docs" in
the project root directory.  Reference materials will be produced in the doc/
folder.  Please note that the tools Doxygen and Graphviz are required to produce
this documentation.

## Test Coverage

This library uses its own unit testing framework to test itself.  Coverage
information is recorded by the program `gcov`, aggregated by the program `lcov`,
and converted into a very nice HTML file by `genhtml`.  This is very useful for
detecting what code needs tests.  To generate test coverage information, run the
following command. at the project root:

    $ make CFG=coverage cov

At which point you can open the file `cov/index.html` in a browser and view the
test coverage info.  Alternatively, you can find the latest test coverage
information on the [libstephen tool site][].

## Plans

My most up-to-date ideas are now maintained on the [GitHub Issues][]

## License

Copyright (c) 2013-2015, Stephen Brennan.  All rights reserved.  Released under
the Revised BSD license.  See [LICENSE.txt][] for details.

[libstephen tool site]: http://stephen-brennan.com/libstephen
[GitHub Issues]: https://github.com/brenns10/libstephen/issues
[GitHub Wiki]: https://github.com/brenns10/libstephen/wiki
[LICENSE.txt]: LICENSE.txt
