Libstephen Tutorial
===================

Welcome to the Libstephen Tutorial! Here you will find documentation about the
major features libstephen has to offer, for general purpose C programming. The
documentation here is not exhaustive. If you really want to know the full
interface of libstephen, there is no substitute for diving into the headers or
the implementations.

The `Libstephen Tool Site <http://stephen-brennan.com/libstephen>`__ has links
to code coverage reports, automatically generated documentation, and the
Wiki. Check it out!.

**Introductory Notes**

libstephen is a personal creation. It's an experiment on the best way to write
an API (for the implementer, and the user -- I am both). That means that the API
could frequently change. I feel like I'm getting close to an implementation that
can stand the test of time, but before I tag a release as 1.0, I don't want to
commit to a steady interface.

Since libstephen is a personal creation, I wouldn't expect that its
implementations are the most efficient ones out there (or bug free). I try to
test the code pretty well, and I use it in other projects, but that doesn't make
it perfect (or even good). There are plenty of libraries out there that provide
basic data structures, which are many times better than mine. I'm doing this for
the pleasure of doing it myself, not for the superiority of my implementations.

If you're still reading, you must be dead set on using libstephen. Here are the
basic ideas of the library:

- A single "generic" type, called ``DATA``. This is an 8-byte union. It can
  contain a very big integer, or a pointer, or a double. You can access these
  through the fields ``data_llint``, ``data_ptr``, and ``data_dbl``
  respectively.
- A simple "object oriented" approach. I'm not pursuing true object oriented
  programming in C, since that's painful. But I create structs, and then write
  functions to operate on them.
- Uniform error handling, through a pointer to a status variable accepted by any
  function that can produce an error (memory allocation errors are not handled).
- A consistent object lifetime, inspired by C++. I like the freedom of being
  able to allocate an object on the stack or heap. Therefore, there are four
  common functions for my objects:

  - ``*_create()`` - allocate object on the heap, initialize it.
  - ``*_init()`` - initialize an already allocated object.
  - ``*_delete()`` - free any resources held by an object, and free the object.
  - ``*_destroy()`` - free resources held by an object, but don't free the
      object.

- Some useful abstractions, inspired by Python and Java:

  - A generic "list" data type, which is implemented by all list data
    structures.
  - A generic "iterator" for iterating over any data structure. Can also be used
    to make generators!

**Miscellaneous**

After having the ``DATA`` union around for so long, I've gotten tired of having
to create a ``DATA`` on the stack, assign a value to it, and then send that to
my functions. So, I created a set of macros which expand to ``DATA`` literals,
so you don't have to do that:

.. code:: c

    DATA an_integer = LLINT(12);
    DATA a_pointer = PTR(NULL);
    DATA a_long_double = DBL(1.2);

    // sample usage
    smb_ll *list = ll_create();
    ll_append(list, LLINT(10));
    ll_append(list, LLINT(20));
    ll_append(list, PTR("this is code for demonstration only!"));

.. toctree::
   :maxdepth: 1

   errors
   arraylist
   linkedlist
   list
   iterator
   hashtable
   charbuf
   args
   logging
   unittest
   regex
   
   
