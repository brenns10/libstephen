Error Handling
==============

libstephen has a standard way to deal with errors in code. Typically,
errors are reported in one of two ways in C code:

- A status value is returned, indicating the success of the function.
- A pointer to a status variable is provided, and is set to indicate the success
  of the function.

libstephen takes the second approach.

Why a pointer to a status variable?
-----------------------------------

I thought long and hard about which approach to take, and I settled on
this for the following reasons:

-  When a function returns a value, you expect it is a "result" of the
   computation. Returning an error code makes it look like the error
   code was something the function computed. This ruins the semantics of
   the function signature.
-  Error codes on return values require reading the documentation on the
   function *every time you call it*. Some may use -1 as the error code,
   others may use 0, or NULL, or some other strategy. Plus, error return
   values reduce the number of possible return values for the actual
   result of the function (what if your function has a legitimate reason
   to return -1?).
-  If a function returns a status, it is laughably easy to just *ignore*
   it by not capturing the result of that expression. When it is an out
   parameter, you *have* to create a variable to store it in, which
   means no ignoring it.

Values
------

The status variable type is just an ``int``, but typedef'd to
``smb_status``. There are a handful of predefined error values:

-  ``SMB_SUCCESS`` - no error
-  ``SMB_INDEX_ERROR`` - index error, for when you try to access an
   index that doesn't exist.
-  ``SMB_NOT_FOUND_ERROR`` - hash key not found in hash table
-  ``SMB_STOP_ITERATION`` - when an iterator stops iterating

Memory allocation errors
------------------------

Memory allocation errors are not included here. It's nearly impossible
to recover from an out of memory error. The code to try to handle them
clutters up the library and program like none other. So, the standard
practice is to just exit when memory allocation fails.

In fact, libstephen provides helper functions for doing exactly this
when allocating memory. Here they are:

- ``type *smb_new(type, size_t amount)``: Allocates ``amount`` of memory of size
  ``size(type)``. Casts to the right pointer type for you. If there is an error,
  prints a message to standard error and terminates with an error value. EG:

  .. code:: C

     char *msg = "hi";
     char *string = smb_new(char, 3);
     strncpy(string, msg, 3);
     printf("%s\n", string);   //STDOUT: hi``

-  ``type *smb_renew(type, type *ptr, size_t newamt)`` - Reallocates the
   buffer to a new size, copying if necessary. Deals with errors for
   you. EG:

   .. code:: C

      string = smb_renew(char, string, 4);
      string[2] = '!';
      string[3] = '\0';
      printf("%s\n", string);   //STDOUT: hi!``

-  ``void smb_free(void *ptr)`` - This actually doesn't do any more than
   ``free()`` does, but it seemed like it was necessary so that
   everything felt aligned. EG:

   .. code:: C

      smb_free(string);

Assertions
----------

Finally, there are plenty of situations when an error should not occur,
but you still have to pass and check a status variable. In those
situations, ``assert()`` is your best friend. Use it only when an error
should never occur. If an error could occur, you should handle the
condition as part of the program logic, even if that means printing an
error message and terminating the program.
