Unit Testing
============

.. code:: c

    #include "libstephen/ut.h"

-  `Header
   (master) <https://github.com/brenns10/libstephen/blob/master/inc/libstephen/ut.h>`__
-  `Implementation
   (master) <https://github.com/brenns10/libstephen/blob/master/src/smbunit.c>`__

libstephen includes a rather spartan unit testing framework. I once
nicknamed it "SmbUnit", and thus all the functions begin with ``su_``.
Meanwhile, the header file is ``libstephen/ut.h``. This is a rather
annoying inconsistency, and I may deal with it before releasing a 1.0.

Anyway, the unit testing framework defines tests, test groups, and test
assertions. There's really no escape from a good deal of boilerplate to
get it to work. But I happen to use YaSnippet in Emacs, so I have
defined auto-expanding snippets for creating tests and groups.

History
-------

One of the motivations behind the unit testing framework is to "keep it
simple, stupid." I figured it would be horrible if I spent more time
debugging my unit testing framework than my actual code, so I made two
decisions:

-  The unit testing framework could not depend on any other part of
   libstephen! No lists!
-  The unit testing framework should try to minimize complex operations,
   like allocating and reallocating memory.

Therefore, the framework has set limits for the size of a test
description (20 characters), and the number of tests in a group (20
tests). These are pretty much guaranteed to never go down, and they may
go up.

At one point, my entire library had the ability to count bytes
allocated, and decrement the counter as bytes were freed. I removed this
feature from the library for three reasons:

-  Tools like Valgrind do this without any change to the code!
-  The changes I needed in order to implement this feature were really
   limiting. Namely, I had to specify how many bytes I was freeing when
   I freed them!
-  Turning off the feature during release builds got rather tricky,
   especially when you started considering how client code would link
   the library.

So, I removed the feature, which meant that tests couldn't detect memory
leaks any more. But I didn't miss it too much, since it's nearly as easy
to run ``valgrind bin/release/test`` as it is to run
``bin/release/test``.

Operations
----------

.. code:: c

    smb_ut_test *su_create_test(char *description, int (*run)());
    smb_ut_group *su_create_test_group(char *description);
    void su_add_test(smb_ut_group *group, smb_ut_test *test);
    int su_run_test(smb_ut_test *test);
    int su_run_group(smb_ut_group *group);
    void su_delete_test(smb_ut_test *test);
    void su_delete_group(smb_ut_group *group);

There are no ``init`` or ``destroy`` operations, pretty much in the name
of keeping it simple. The basic operation is that you create a group,
create tests, add the tests to the group, and run the group (which runs
each individual test). Then you delete the group (which deletes each
individual test).

Test functions take void, and return an int. The return value should be
0 on success, or some non-zero value on failure. This value is typically
the line number of the assertion that failed.

Test assertions are done with the macro ``TEST_ASSERT(condition);``. On
failure, it returns the current line number.

Sample Use
----------

Here is a sample from some of libstephen's unit tests:

.. code:: c

    smb_list (*get_list)(void);

    int test_append()
    {
      /* ... */
      TEST_ASSERT(/* test condition */);
      return 0;
    }

    int test_prepend()
    { /* ... */ }

    /* ...
       ...
       ... */

    void run_list_tests(char *desc)
    {
      smb_ut_group *group = su_create_test_group(desc);

      smb_ut_test *append = su_create_test("append", test_append);
      su_add_test(group, append);

      smb_ut_test *prepend = su_create_test("prepend", test_prepend);
      su_add_test(group, prepend);

      /* ...
         ...
         ... */

      su_run_group(group);
      su_delete_group(group);
    }

    void list_test(void)
    {
      get_list = &ll_create_list;
      run_list_tests("list_ll");

      get_list = &al_create_list;
      run_list_tests("list_al");
    }

This is a rather advanced example that shows some cool ways to deal with
lacks of test setup and teardown functions. These tests are designed to
run on array lists and linked lists. There is a global ``get_list()``
function pointer that is initially set to be linked list, and then set
to be array list. The test functions use this to get a UUT.

Anyhow, the mean features and uses are demonstrated under the function
``run_list_tests()``.
