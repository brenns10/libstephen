List Interface
==============

.. code:: C

    #include "libstephen/list.h"

-  `Header
   (master) <https://github.com/brenns10/libstephen/blob/master/inc/libstephen/list.h>`__

The list interface, defined in ``libstephen/list.h``, is designed to be
an implementation-agnostic interface to a list. It comes at a very high
price. It is a struct containing a large set of function pointers, and a
pointer to data. This means that you have a powerful, full-featured
interface to the list. However, frequently using this interface is
probably inefficient.

Operations
----------

.. code:: C

    typedef struct smb_list
    {
      void *data;
      void (*append)(struct smb_list *l, DATA newData);
      void (*prepend)(struct smb_list *l, DATA newData);
      DATA (*get)(const struct smb_list *l, int index, smb_status *status);
      void (*set)(struct smb_list *l, int index, DATA newData, smb_status *status);
      void (*remove)(struct smb_list *l, int index, smb_status *status);
      void (*insert)(struct smb_list *l, int index, DATA newData);
      void (*delete)(struct smb_list *l);
      int (*length)(const struct smb_list *l);
      void (*push_back)(struct smb_list *l, DATA newData);
      DATA (*pop_back)(struct smb_list *l, smb_status *status);
      DATA (*peek_back)(struct smb_list *l, smb_status *status);
      void (*push_front)(struct smb_list *l, DATA newData);
      DATA (*pop_front)(struct smb_list *l, smb_status *status);
      DATA (*peek_front)(struct smb_list *l, smb_status *status);
      int (*index_of)(const struct smb_list *l, DATA d, DATA_COMPARE comp);
    } smb_list;

These operations are most of the useful features of the array and linked
lists.

Sample Usage
------------

The following code is straight from one of my simpler tests for lists.
It works with linked lists or array lists (but a linked list is created
here). Each iteration the following code adds a number to the list, and
then verifies that the list contains the expected values.

.. code:: C

    smb_status status = SMB_SUCCESS;
    DATA d;
    smb_list list = ll_create_list();

    for (d.data_llint = 0; d.data_llint < 200; d.data_llint++) {
      list.append(&list, d);
      TEST_ASSERT(list.length(&list) == d.data_llint + 1);

      for (int i = 0; i < list.length(&list); i++) {
        TEST_ASSERT(list.get(&list, i, &status).data_llint == i);
        TEST_ASSERT(status == SMB_SUCCESS);
      }
    }

    list.delete(&list);
