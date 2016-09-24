Libstephen Tutorial
===================

**Introductory Notes**

Here are some basic ideas in the design of the library.

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

**Tutorial**

So, now that you know a little bit about the ideas behind my library, how do you
get it up and running in your project?  Well, there are quite a few ways, but
the best way currently is to use a Git submodule.

I'll assume you already have a Git repository with some C code.  Then, to add in
libstephen, you'll want to do:

.. code:: bash
   git submodule add https://github.com/brenns10/libstephen.git libstephen

This will add a ``libstephen`` directory to your project, and check out the
current version of libstephen.  Next, you'll want to add
``libstephen/bin/release/libstephen.a`` as a dependency for your main executable
in your Makefile.  You'll also want to create a rule that will build libstephen.
Imagine you had an existing makefile that looked like this:

.. code:: Makefile

   bin/release/main: $(OBJECTS)
       gcc -o bin/release/main $(OBJECTS)

Then, you could add in libstephen to the build process as simply as this:

.. code:: Makefile

   libstephen/bin/release/libstephen.a:
       make -C libstephen

   bin/release/main: $(OBJECTS) libstephen/bin/release/libstephen.a
       gcc -o bin/release/main $(OBJECTS) libstephen/bin/release/libstephen.a

The only other thing is you'll want to make sure that ``libstephen/inc`` is in
your include path.  This is as simple as providing that path in the ``-I``
option to GCC.

**Miscellaneous**

After having the ``DATA`` union around for so long, I've gotten tired of having
to create a ``DATA`` on the stack, assign a value to it, and then send that to
my functions. So, I created a set of macros which expand to ``DATA`` literals,
so you don't have to do that:

.. code:: C

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
   
   
