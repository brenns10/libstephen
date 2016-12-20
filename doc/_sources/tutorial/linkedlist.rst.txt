Linked List
===========

.. code:: C

    #include "libstephen/ll.h"

-  `Header
   (master) <https://github.com/brenns10/libstephen/blob/master/inc/libstephen/ll.h>`__
-  `Implementation
   (master) <https://github.com/brenns10/libstephen/blob/master/src/linkedlist.c>`__

The linked list struct (``smb_ll``) is a doubly-linked list. It is made
up of a chain of nodes. Each node contains a ``DATA``, as well as links
to the next and previous nodes.

Operations
----------

Like all other objects, ``smb_ll`` supports the four basic
creation/deletion functions. They create new, empty lists. The deletion
functions delete the struct and all nodes, but they won't free any
pointers contained in DATA, naturally.

Here are some operations supported by ``smb_ll``

.. code:: C

    void ll_append(smb_ll *list, DATA newData);
    void ll_prepend(smb_ll *list, DATA newData);
    void ll_push_back(smb_ll *list, DATA newData);
    DATA ll_pop_back(smb_ll *list, smb_status *status);
    DATA ll_peek_back(smb_ll *list, smb_status *status);
    void ll_push_front(smb_ll *list, DATA newData);
    DATA ll_pop_front(smb_ll *list, smb_status *status);
    DATA ll_peek_front(smb_ll *list, smb_status *status);
    DATA ll_get(const smb_ll *list, int index, smb_status *status);
    void ll_remove(smb_ll *list, int index, smb_status *status);
    void ll_insert(smb_ll *list, int index, DATA newData);
    void ll_set(smb_ll *list, int index, DATA newData, smb_status *status);
    int  ll_length(const smb_ll *list);
    int  ll_index_of(const smb_ll *list, DATA d, DATA_COMPARE comp);

This is directly from the header. The append and prepend functions are
exactly what you'd expect. The push/pop/peek functions make the list act
like a stack. The front ones act on the beginning of the list, and the
back ones operate on the end of the list. So ``push_back`` is the same
as ``append``. ``get``, ``remove``, ``insert``, and ``set`` provide your
basic list access and modification functions. ``length`` tells you the
length of the list (*Don't directly access the field, I don't make any
guarantee that the structs will remain the same. They're implementation
details!*). ``index_of`` will find the first occurrence of a value, by a
linear search.

You can create generic lists with the following functions:

.. code:: C

    smb_list ll_create_list();
    smb_list ll_cast_to_list(smb_ll *list);

And you can get an iterator for a list with this function:

.. code:: C

    smb_iter ll_get_iter(const smb_ll *list);

Sample Usage
------------

Using the linked list is pretty simple. Here, I create a list, add some
numbers to it, and then free it.

.. code:: C

    smb_ll *list = ll_create();

    ll_append(list, LLINT(0));
    ll_append(list, LLINT(1));

    ll_free(list);

Functional Programming!
-----------------------

One of the more recent additions to my linked list API is some high-level
functions for lists - namely, map, filter, and fold.  My implementations are all
in-place for efficiency.  Here they are:

.. code:: C

   void ll_filter(smb_ll *list, bool (*test_function)(DATA));
   void ll_map(smb_ll *list, DATA (*map_function)(DATA));
   DATA ll_foldl(smb_ll *list, DATA start_value, DATA (*reduction)(DATA,DATA));
   DATA ll_foldr(smb_ll *list, DATA start_value, DATA (*reduction)(DATA,DATA));

Check the API documentation for full details.

Structure
---------

The following items are implementation details, and not really necessary
to use the linked list. Here is the structure of a node:

.. code:: C

    typedef struct smb_ll_node
    {
      struct smb_ll_node *prev;
      struct smb_ll_node *next;
      DATA data;
    } smb_ll_node;

And here is the actual linked list struct:

.. code:: C

    typedef struct smb_ll
    {
      struct smb_ll_node *head;
      struct smb_ll_node *tail;
      int length;
    } smb_ll;
