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

### Tools

- Unit testing framework (su)
- Command line argument parser

## Documentation

Technical documentation on the source code and functions is available as
documentation comments in source code.  This can be put into a more readable
form by running the command "make docs" in the project root directory.
Reference materials will be produced in the doc/ folder.  Please note that the
tools Doxygen and Graphviz are required to produce this documentation.

## Test Coverage

This library uses its own unit testing framework to test itself.  Coverage
information is recorded by the program `gcov`, aggregated by the program `lcov`,
and converted into a very nice HTML filed by `genhtml`.  This is very useful for
detecting what code needs tests.  To generate test coverage information, run the
following commands at the project root:

    $ make CFG=coverage test
    $ bin/coverage/test
    $ make gcov

At which point you can open the file `cov/index.html` in a browser and view the
test coverage info.

Test coverage is generated in *release* mode, not debug mode.  This is because
you don't want to test a version that is different from the one that you ship.
And, since test coverage in both modes is a duplication of effort, I only
support it in release mode.

## Contributing

If anyone feels so generous as to contribute in any way, it is always
appreciated.  Bug reports are welcome, as well as code improvements in the form
of pull requests.  All these things are available at the project homepage:
<https://github.com/brenns10/libstephen>

## Plans

For information on future plans for the library, see `TODO.md`.

## License

Copyright (c) 2013-2015, Stephen Brennan.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
  * Neither the name of Stephen Brennan nor the names of his contributors may
    be used to endorse or promote products derived from this software without
    specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL STEPHEN BRENNAN BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
