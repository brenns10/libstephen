Embedding the Interpreter
=========================

The REPL
--------

With a working understanding of the basics of the reference counting system, the
basic read-eval-print loop for the interpreter is simple. Taking it in steps:

1. Read a line of input.
2. Parse the input. Parsed code is simply a ``lisp_value`` like any other
   language object, and so the REPL now owns a reference to the input.
3. Free the line of input.
4. Evaluate the input. Similar to the above, the REPL now owns references to the
   result.
5. Decref the code, since it is no longer needed.
6. Print the output, and a trailing newline.
7. Decref the output, since it is no longer needed.

One major issue is overlooked in these steps: scope. The scope is simply a
mapping of names (i.e. ``lisp_symbol`` objects) to values (any ``lisp_value``).
A scope is required to evaluate any code. The scope contains most of the
critical builtin functions for lisp, such as ``car``, ``cdr``, ``cons``, etc.
The scope may also be modified by the builtin function ``define``, so that code
can bind a name to a value it creates (such as a function).

Before an interpreter can evaluate any code, it must instantiate a scope and
populate it with the basic built-in functions for lisp. Once the interpreter has
exited, we can decref it, which should free every key and value that was stored
in the scope.

With the scope in mind, here is some basic code that demonstrates embedding a
simple lisp interpreter, without any custom functions. It uses the ``editline``
implementation of the ``readline`` library for reading input (and allowing line
editing).

.. code:: C

   #include <editline/readline.h>
   #include <stdio.h>

   #include "libstephen/lisp.h"

   int main(int argc, char **Argo)
   {
     lisp_scope *scope = (lisp_scope*)type_scope->new();
     lisp_scope_populate_builtins(scope);

     // Add your own builtins here?

     while (true) {
       char *input = readline("> ");
       if (input == NULL) {
         break;
       }
       lisp_value *value = lisp_parse(input);
       add_history(input); // for editline history only
       free(input);
       lisp_value *result = lisp_eval(scope, value);
       lisp_decref(value);
       lisp_print(stdout, result);
       fprintf(stdout, "\n");
       lisp_decref(result);
     }

     lisp_decref((lisp_value*)scope);
     return 0;
   }

Writing Builtins
----------------

Typically, an embedded interpreter will not be of much use to your application
unless you can also add functions to the global scope. The most straightforward
way to add your own functionality to the interpreter is by writing a "builtin".
This is a C function which may be called by lisp code. Builtins must have the
following signature:

.. code:: C

   lisp_value *lisp_builtin_somename(lisp_scope *scope, lisp_value *arglist);

The scope argument contains the current binding of names to values, and the
arglist is a list of arguments to your function, which **have not been
evaluated**. These arguments are essentially code objects. You'll almost always
want to evaluate them all before continuing with the logic of the function. You
can do this individually with the ``lisp_eval()`` function, or just evaluate the
whole list of arguments with the ``lisp_eval_list()`` function.

The one exception to evaluating all of your arguments is if you're defining some
sort of syntactic construct. For instance, if you were to write a builtin
function for an if-statement, you would evaluate the expression, and then
conditionally evaluate one of the two other expressions (but not both) depending
on the value of the expression.

Finally, when you have your argument list, you could verify them all manually,
but this process gets annoying very fast. To simplify this process, there is
``lisp_get_args()``, a function which takes a list of (evaluated or unevaluated)
arguments and a format string, along with a list of pointers to result
variables. Similar to ``Scan()``, it reads a type code from the format string
and attempts to take the next object off of the list, verify the type, and
assign it to the current variable in the list. The current format string
characters are:

- ``d``: for integer
- ``l``: for list
- ``s``: for symbol
- ``S``: for string
- ``o``: for scope
- ``e``: for error
- ``b``: for builtin
- ``t``: for type
- ``*``: for anything

So, a format string for the plus function would be ``"dd"``, and the format
string for the ``cons`` function is ``"**"``, because any two things may be put
together in an s-expression. If nothing else, the ``lisp_get_args()`` function
can help you verify the number of arguments, if not their types. When it fails,
it returns false, which you should typically handle by returning an error
(``lisp_error_new()``). You should be sure to decref anything you have allocated
as you parsed your arguments (such as the return value from
``lisp_eval_list()``). Even when an error has occurred!

At this point, your function is free to do whatever logic you'd like. Keep in
mind that the return value of ``lisp_eval_list()`` must be decref'd when you're
done with it, and also that your return values must be new references.

Basics of Lisp Types
--------------------

In order to write any interesting functions, you need a basic idea of how types
are represented and how you can get argument values out of the ``lisp_value``
objects. This is not a description of the type system (a future page in this
section will cover that), just a list of available types and their values.

The current types (that you are likely to use) are:

- ``lisp_list``: contains a ``left`` and a ``right`` pointer.

  - ``left`` is usually a value of the linked list, and ``right`` is usually the
    next list in the linked list. However this isn't necessarily the case,
    because this object really represents an s-expression, and the right value
    of an s-expression doesn't have to be another s-expression.
  - The empty list is a special instance of ``lisp_list``. You can get a new
    reference to it with ``lisp_nil_new()`` and you can check if an object is
    nil by calling ``lisp_nil_p()``.
  - You can find the length of a list by using ``lisp_list_length()``.

- ``lisp_symbol``: type that represents names. Contains ``sym``, which is a
  ``char*``.
- ``lisp_error``: similar to a symbol in implementation, but represents an
  error. Has the attribute ``message`` which contains the error message. Create
  a new one with ``lisp_error_new(message)``.
- ``lisp_integer``: contains attribute ``x``, an integer. Yes, it's allocated on
  the heap.  Get over it.
- ``lisp_string``: another thing similar to a symbol in implementation, but this
  time it represents a language string literal. The ``s`` attribute holds the
  string value.

There are also types for builtin functions, lambdas, scopes, and even a type for
types!  But you probably won't use them in your average code.

The following functions can be called on any lisp type (they may raise errors if
not applicable):

- ``lisp_print(FILE *f, lisp_value *v)``
- ``lisp_eval(lisp_scope *s, lisp_value *v)``
- ``lisp_call(lisp_scope *s, lisp_value *callable, lisp_value *arguments)``
  - invokes ``callable`` on ``arguments`` in scope ``s``.
- Of course, ``lisp_incref`` and ``lisp_decref``. There is also ``lisp_free``,
  but that shouldn't ever be used.

Adding Builtins to the Scope
----------------------------

Once you have written your functions, you must finally add them to the
interpreter's global scope. Anything can be added to a scope with
``lisp_scope_bind()``., but the name needs to be a ``lisp_symbol`` instance and
the value needs to be a ``lisp_value``. To save you the trouble of creating
those objects, you can simply use ``lisp_scope_add_builtin()``, which takes a
scope, a string name, and a function pointer.

Here is a code example that puts all of this together, based on the REPL given
above.

.. code:: C

   #include <editline/readline.h>
   #include <stdio.h>

   #include "libstephen/lisp.h"

   static lisp_value *say_hello(lisp_scope *scope, lisp_value *a)
   {
     // Evaluate our arguments.
     lisp_value *arglist = lisp_eval_list(scope, a);

     // Check our number and type of arguments.
     lisp_string *s;
     if (!lisp_get_args((lisp_list*)arglist, "S", &s)) {
       // Don't forget to decref on error.
       lisp_decref(arglist);
       return (lisp_value*)lisp_error_new("error: expected a string!");
     }

     // Perform our logic.
     printf("Hello, %s!\n", s->s);

     // Decref the evaluated arguments, which we owned.
     lisp_decref(arglist);

     // we have to return something...
     return (lisp_value*) lisp_nil_new();
   }

   int main(int argc, char **argv)
   {
     lisp_scope *scope = (lisp_scope*)type_scope->new();
     lisp_scope_populate_builtins(scope);

     lisp_scope_add_builtin(scope, "hello", say_hello);

     while (true) {
       char *input = readline("> ");
       if (input == NULL) {
         break;
       }
       lisp_value *value = lisp_parse(input);
       add_history(input);
       free(input);
       lisp_value *result = lisp_eval(scope, value);
       lisp_decref(value);
       lisp_print(stdout, result);
       fprintf(stdout, "\n");
       lisp_decref(result);
     }

     lisp_decref((lisp_value*)scope);
     return 0;
   }


An example session using the builtin:

.. code::

   > (hello "Stephen")
   Hello, Stephen!
   ()
   > (hello 1)
   error: error: expected a string!
   > (hello 'Stephen)
   error: error: expected a string!
