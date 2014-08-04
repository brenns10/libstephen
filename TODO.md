# libstephen To-Do List

I already find `libstephen` to be a useful tool for my general purpose C
programming.  I use it extensively in my regular expression and parser project,
[`cky`](https://bitbucket.org/brenns10/cky).  The data structure support and
memory allocation tracking features are invaluable every day necessities for
writing C code that I'm comfortable with.  However, there is always room for
improvement.  Most definitely, my library is far from perfect, and I want to
bring it as close as possible to perfection.  As such, here are some of my plans
for the future.

## Iterators

I already have some sort of iterator support in `libstephen`.  However, it only
exists for linked lists (since the other data structures support constant time
iteration).  However, I have found that programming languages like Python take
iterators to nearly an art form.  If you can write code that exclusively uses
iterators for iterating, you can swap out data structures easily.  In fact, you
can even create generators that act as iterators!

This flexibility seems uncommon in C.  But, I aim to integrate it as best as I
can with iterators.  I plan to create a new, better iterator data structure that
is generic.  It will be a struct similar to the following:

```
#!c
typedef struct {
  DATA ds;
  DATA state;
  DATA (*next)(smb_iter *);
  bool (*has_next)(smb_iter *);
  void (*destroy)(smb_iter *, bool free_src);
  void (*delete)(smb_iter *, bool free_src);
  /* possibly more functions */
} smb_iter;
```

This way, each data structure may populate its own functions.  The iterator
struct will have space for a pointer to the data structure, as well as a pointer
or value for its state.  It will have your average forward iteration functions,
along with memory management functions for freeing resources held by the
iterator, and optionally the data structure it points to.

Array list, linked list, and hash table iterators could be easily implemented
like this, as well as generators.

Note that function invocation on an iterator would be a bit awkward.  For
instance: `iter->next(&iter)`.  I could make a macro to 'simplify' it, like so:
`CALL(iter, next, args)`, but that seems like it's obfuscating the meaning of
the code a bit much, just to save a few characters.

## Better Error Handling

This is a big one.  I currently have a rather half-assed attempt at error
handling right now.  It consists of an error variable with a limited amount of
error flags, and no more information.  It requires the user to be extremely
proactive (`if (CHECK(ERROR_X)) { /* handle error */}`).  I need an error
handling mechanism that is easier to use, includes more information, and can
drop a stack trace when necessary.  Of course, I can't implement try/catch
blocks (well, it's possible with setjmp/longjmp... but terrifying), but I can
make things much easier.

## Wrapped Standard Library Calls

Along with better error handling would be wrapped stdlib calls.  They could
convert stdlib errors into `libstephen` errors, and possible handle some of them
(like printing a stack trace, exiting, etc.).

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
