# libstephen To-Do List

I already find `libstephen` to be a useful tool for my general purpose C
programming.  I use it extensively in my regular expression and parser project,
[`cky`](https://bitbucket.org/brenns10/cky).  The data structure support is an
invaluable every day necessitiy for writing C code that I'm comfortable with.
However, there is always room for improvement.  Most definitely, my library is
far from perfect, and I want to bring it as close as possible to perfection.  As
such, here are some of my plans for the future.

## Test Coverage

Now that I've settled on a good error handling scheme, I'd like to up my test
coverage significantly.

## Release 1.0

This sounds silly, but lately I've done a lot of API breaking changes.  I want
to ensure that this doesn't happen, so my dowstream projects don't have to drown
in refactoring.  Therefore, I need to make any more API decisions I want
quickly, and then commit to maintaining that API forever.

## Integrate Dowstream Projects

CKY has a regular expression library that is mostly complete, but needs testing.
Once that is complete, it should be moved into Libstephen.  Possibly likewise
with the grammar/parsing tools too.  Also, I wrote a shell called LSH, with the
intention of integrating it with Libstephen.  If I get a solid syntax working
with it, I can migrate that into Libstephen as well.

## Unit Test Setup and Tear Down

Most unit testing libraries have setup and tear down methods.  I can make
group-level and test-level setup and tear down methods.

## String Functions

It would be nice to have functions that work on `char *` strings, that provide
functionality similar to some of Python's string handling capabilities.  For
instance, substrings, split, etc.
