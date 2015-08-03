Character Buffer
================

.. code:: c

    #include "libstephen/cb.h"

-  `Header
   (master) <https://github.com/brenns10/libstephen/blob/master/inc/libstephen/cb.h>`__
-  `Implementation
   (master) <https://github.com/brenns10/libstephen/blob/master/src/charbuf.c>`__

Unlike pretty much every other language under the sun, C has
exceptionally few string handling functions available to you. What's
worse, every other language typically hides a whole lot of complexity in
its string type. Most lanugages have immutable strings, which means you
can't modify them without reallocating them. While this has plenty of
advantages, it's also terribly inefficient when you're doing text
processing. Languages with immutable strings frequently provide a string
"buffer" or "builder" abstraction, which is essentially a mutable string
that has support for lots of modification actions.

Since C doesn't have immutable strings, you need to implement a lot of
the "complexity" that other languages provide for efficient operations
on strings. This character buffer abstraction exists to remove some of
the common complexity with dealing with strings in C. Namely, it does
the allocation and dynamic reallocation that you would normally have to
tackle when you're dealing with buffers - especially when reading input!

Functions
---------

Here is the struct definition and list of functions:

.. code:: c

    typedef struct {
      char *buf;
      int capacity;
      int length;
    } cbuf;

    void cb_init(cbuf *obj, int capacity);
    cbuf *cb_create(int capacity);
    void cb_destroy(cbuf *obj);
    void cb_delete(cbuf *obj);

    void cb_concat(cbuf *obj, char *str);
    void cb_append(cbuf *obj, char next);
    void cb_trim(cbuf *obj);
    void cb_clear(cbuf *obj);
    void cb_printf(cbuf *obj, char *format, ...);
    void cb_vprintf(cbuf *obj, char *format, va_list va);

The lifetime functions are self-explanatory, but here's a little
commentary anyway. You really shouldn't have to use ``cb_create()`` that
often. For the most part, you'll want to declare a ``cbuf`` on the stack
and use ``cb_init()``. This is because most use cases for a ``cbuf``
involve building a string over the course of a single function call. I
don't recommend passing around ``cbuf``\ s, treating them as strings.

Also, note that calling ``cb_destroy()`` is **optional**! Generally,
you'll want to keep the created string longer than you'll want to keep
the ``cbuf`` around. The buffer is accessible through the ``buf`` field
of the struct. Whenever you'd like, you can stop using the ``cbuf`` and
start passing around the underlying buffer like a normal string.

Anyway:

-  ``concat`` - add a string to the end.
-  ``append`` - add a character to the end.
-  ``trim`` - reallocate to just the length of the string. Useful once
   you've created the string and you want to stop using the ``cbuf``.
-  ``clear`` - clear out the whole buffer. This does nothing accept
   reset ``buf[0]`` to ``\0`` and reset ``length`` to ``0``. It doesn't
   actually "erase" the buffer.
-  ``printf`` - print a format string onto the buffer. ``vprintf`` does
   the same, but takes a ``va_list``.

Wide Character Version
----------------------

In case you're unfamiliar, C has a ``wchar_t`` type, which is capable of
holding any Unicode character. Some text processing tasks may require
you to work with the underlying characters/code points rather than the
text in some encoding. I have implemented the exact same interface, but
for ``wchar_t *`` buffers. Everything is the same, except that the type
is ``wcbuf``, the functions are prefixed by ``wcb_``, and the text
arguments are always ``wchar_t *`` instead of ``char *``.

I/O Utilities
-------------

.. code:: c

    #include "libstephen/str.h"

-  `Header
   (master) <https://github.com/brenns10/libstephen/blob/master/inc/libstephen/str.h>`__
-  `Implementation
   (master) <https://github.com/brenns10/libstephen/blob/master/src/string.c>`__

Using the character buffer, I've implemented a few simple tasks quite
easily. Here are some functions you might find useful:

.. code:: c

    char *read_file(FILE *f);
    wchar_t *read_filew(FILE *f);
    char *read_line(FILE *file);
    wchar_t *read_linew(FILE *file);
    smb_ll *split_lines(char *source);
    smb_ll *split_linesw(wchar_t *source);

``split_lines`` isn't actually implemented with a ``cbuf`` (you don't
need one), but I'm including it because I don't have an official "String
Handling" page yet.
