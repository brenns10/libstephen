Argument Parsing
================

.. code:: C

    #include "libstephen/ad.h"

-  `Header
   (master) <https://github.com/brenns10/libstephen/blob/master/inc/libstephen/ad.h>`__
-  `Implementation
   (master) <https://github.com/brenns10/libstephen/blob/master/src/args.c>`__

For practical programs, argument parsing is a large part of what you do.
So, having a simple library to help you build normal command line
interfaces can save you plenty of time. The library in
``libstephen/ad.h`` provides a basic argument parsing solution. I wrote
it before I knew about the existence of well established solutions like
``getopt``. It's not a perfect solution, but it does the job for most of
my use cases.

Argument Types
--------------

My argument processor is based on four different types of arguments:

-  Short flags: these are one letter flags that can be on or off. They
   are prefixed by a single hyphen. They can also be smooshed together.
   For instance, with the command ``ls -la`` has two short flags: ``l``
   and ``a``.
-  Long flags: these are multi-letter flags that can be on or off. They
   have to be prefixed by a double hyphen. So, the command ``ls --all``
   has a long flag: ``all``.
-  Parameters: both short and long flags may have parameters provided to
   them. For instance, ``gcc -o code.o code.c`` has ``code.o`` provided
   as an argument to the short flag ``o``.
-  Bare strings: these are neither short nor long flags. They are simply
   arguments provided to the program. They are typically the actual
   objects that the program is to operate on (input files, or input
   text). For instance, ``gcc   -o code.o code.c`` has the bare string
   ``code.c``.

Initialization
--------------

.. code:: C

    void arg_data_init(smb_ad *data);
    smb_ad *arg_data_create();
    void arg_data_destroy(smb_ad *data);
    void arg_data_delete(smb_ad *data);

    void process_args(smb_ad *data, int argc, char **argv);

In order to start using the arg parsing library, you create a ``smb_ad``
object. Typically you do this on the stack in your ``main()`` function,
because why would your arguments need to last any longer than that? You
call ``arg_data_init()`` with a pointer to the ``smb_ad`` to initialize
it. To parse your program's arguments, you call
``process_args(smb_ad *, int argc, char **argv)``. You shouldn't include
the program name in ``argc`` and ``argv`` (unless you want it to be
parsed as an argument).

So, something like this would be the basic structure of your main
function:

.. code:: C

    int main(int argc, char *argv[])
    {
      smb_ad args;
      arg_data_init(&args);
      process_args(&args, argc-1, argv+1);

      // Get arguments and flags

      // Do program logic

      arg_data_destroy(&args);
      return EXIT_SUCCESS;
    }

Reading Argument Data
---------------------

.. code:: C

    int check_flag(smb_ad *data, char flag);
    int check_long_flag(smb_ad *data, char *flag);
    int check_bare_string(smb_ad *data, char *string);
    char *get_flag_parameter(smb_ad *data, char flag);
    char *get_long_flag_parameter(smb_ad *data, char *string);

Once you have processed the argument data, the ``smb_ad`` can be queried
for each argument you're expecting. Typically, you'll pair short flags
with long flags (e.g. ``-h`` and ``--help``). You can check for long
flags with ``check_long_flag()``, and you can check for short flags with
``check_flag()``. You can get flag and long flag parameters with
``get_flag_parameter()`` and ``get_long_flag_parameter()`` respectively.
You can also use ``check_bare_string()`` to check if a bare string was
provided. However, that's not usually the most convenient way to access
the bare strings (since you probably don't already know the bare string
before it's provided). So, you can also access the linked list directly
in the ``bare_strings`` field of the ``smb_ad`` struct.

Here's a typical example of how you might use ``smb_ad`` to parse
arguments. It continues from the outline earlier.

.. code:: C

    if (check_flag(&args, 'h') || check_long_flag(&args, "help")) {
      help();
    } else if (check_flag(&args, 'o') || check_long_flag(&args, "output")) {
      output = get_flag_parameter(&args, 'o');
      if (!output) {
        output = get_long_flag_parameter(&args, "output");
      }
    }

    // Now you can do stuff with the output file name you got!
