# libstephen To-Do List

I already find `libstephen` to be a useful tool for my general purpose C
programming.  I use it extensively in my regular expression and parser project,
[`cky`](https://bitbucket.org/brenns10/cky).  The data structure support and
memory allocation tracking features are invaluable every day necessities for
writing C code that I'm comfortable with.  However, there is always room for
improvement.  Most definitely, my library is far from perfect, and I want to
bring it as close as possible to perfection.  As such, here are some of my plans
for the future.

## Unit Test Setup and Tear Down

Most unit testing libraries have setup and tear down methods.  I can make
group-level and test-level setup and tear down methods.

## Nice String Features

### String Builders

I'd like a way to easily construct strings.  A string builder that handles the
automatic copying and reallocation (similar to an array list, but more efficient
for chars, and with more string-oriented functionality) would be really welcome.

### String Handlers

It would also be nice to have functions that work on `char *` strings, that
provide functionality similar to some of Python's string handling capabilities.
For instance, substrings, a split function, etc.

## "Pickling" Support

This is a far-off possibility.  But, it would be neat to allow data structures
to be "pickled" to a binary file.  It would take a lot of doing, but it may be
possible to follow all pointers via a sort of breadth-first search, and reassign
them to locations in files, and then save all the data to a file.  This is a bit
crazy, but it /might/ be possible.
