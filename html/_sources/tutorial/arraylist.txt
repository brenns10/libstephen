Array List (smb_al)
===================

.. code:: c

    #include "libstephen/al.h"

-  `Header
   (master) <https://github.com/brenns10/libstephen/blob/master/inc/libstephen/al.h>`__
-  `Implementation
   (master) <https://github.com/brenns10/libstephen/blob/master/src/arraylist.c>`__

The array list (``smb_al``) is a dynamically expanding array of
``DATA``. Its benefits over a linked list are constant time access, with
the drawback of linear time insertion. Data is more localized in array
lists, but must be copied from time to time as the list expands.

Operations
----------

The array list supports the four normal creation and deletion functions,
which create an empty list. Here are some array list operations:

.. code:: c

    void al_append(smb_al *list, DATA newData);
    void al_prepend(smb_al *list, DATA newData);
    DATA al_get(const smb_al *list, int index, smb_status *status);
    void al_remove(smb_al *list, int index, smb_status *status);
    void al_insert(smb_al *list, int index, DATA newData);
    void al_set(smb_al *list, int index, DATA newData, smb_status *status);
    void al_push_back(smb_al *list, DATA newData);
    DATA al_pop_back(smb_al *list, smb_status *status);
    DATA al_peek_back(smb_al *list, smb_status *status);
    void al_push_front(smb_al *list, DATA newData);
    DATA al_pop_front(smb_al *list, smb_status *status);
    DATA al_peek_front(smb_al *list, smb_status *status);
    int al_length(const smb_al *list);
    int al_index_of(const smb_al *list, DATA d, DATA_COMPARE comp);

These operations are identical to the corresponding ones on the linked
list. Appending is done in constant time, except when the array must be
expanded. Insertions (including prepend) are linear time, while get/set
operations are constant.

These will get you a generic list:

.. code:: c

    smb_list al_create_list();
    smb_list al_cast_to_list(smb_al *list);

And this will create an iterator:

.. code:: c

    smb_iter al_get_iter(const smb_al *list);

Sample Usage
------------

This is some sample code identical to the one I used to show the linked
list:

.. code:: c

    smb_al *list = al_create();
    DATA d;

    d.data_llint = 0;
    al_append(list, d);
    d.data_llint = 1;
    al_append(list, d);

    al_delete(list);

Structure
---------

*Here be implementation details!*

Here is the structure of an ``smb_al`` struct:

.. code:: c

    typedef struct smb_al
    {
      DATA *data;
      int length;
      int allocated;
    } smb_al;

The ``data`` pointer points to the dynamically-allocated array of
``DATA``. ``length`` contains the number of items in the list, while
``allocated`` indicates how many ``DATA`` are currently allocated in the
block pointed by ``data``.
