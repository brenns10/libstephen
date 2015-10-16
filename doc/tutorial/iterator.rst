Iterator
========

.. code:: C

    #include "libstephen/list.h"

-  `Header
   (master) <https://github.com/brenns10/libstephen/blob/master/inc/libstephen/list.h>`__

The iterator struct (``smb_iter``) is my reaction to the high overhead
of the list interface. The list interface allows for some powerful
testing and more generic code, but it is frequently more than you really
need. Even more importantly, the list interface doesn't provide linear
time iteration on linked lists (iterating via ``get`` is ``O(N^2)``).

So, the iterator aims to provide a read-only, iterate-once interface to
getting data from a data structure. It has a drastically reduced set of
features, making it much more space efficient. Additionally, iterators
can be implemented for anything, not just data structures. They can be
used to implement generators, and use them just like lists (this is
*heavily* influenced by Python).

Here are some of my future ideas for iterator/generator implementations:

- Character iterators. They can be used to iterate over a string or file. They
  can be used to automatically convert a UTF-8 input stream to a sequence of
  Unicode ``wchar_t`` characters.
- Mathematically inspired generators could be useful.

Plus, functions operating on iterators are very generic:

- Printer function would print the items of a list.
- Copy functions could copy data from an iterator into a new list.

Structure & Function
--------------------

The iterator's structure and function are tied together. It's a bit
difficult to use one without the other.

.. code:: C

    typedef struct smb_iter {
      const void *ds;
      DATA state;
      int index;
      DATA (*next)(struct smb_iter *iter, smb_status *status);
      bool (*has_next)(struct smb_iter *iter);
      void (*destroy)(struct smb_iter *iter);
      void (*delete)(struct smb_iter *iter);
    } smb_iter;

There are three data fields, which have roughly defined purposes. The
``ds`` field should hold a pointer to the data structure. The ``state``
field can hold a number or a pointer as part of the iterator's state.
The ``index`` field can also hold a number, that is usually the index of
the iteration (but doesn't have to be).

The ``next`` and ``has_next`` functions are pretty obvious. The ``next``
function might raise a ``SMB_STOP_ITERATION`` error if you don't call
``has_next`` first. The ``destroy`` and ``delete`` functions are a bit
more difficult. Since an iterator is a read-only view on a data
structure, it doesn't really make sense to free the underlying data
structure when you destroy an iterator. So, ``destroy`` doesn't usually
do anything, and ``delete`` just ``destroy``\ s the iterator, and then
frees it. The reason they do exist is so that new and exciting iterator
implementations can have support for freeing up resources if necessary.

Sample Usage
------------

Here is a typical use of an iterator:

.. code:: C

    smb_iter it = // something returning an iterator to a list of numbers
    DATA d;

    while (it.has_next(&it)) {
      d = it.next(&it);
      printf("List element: %ld\n", d.data_llint);
    }
    it.destroy(&it);
