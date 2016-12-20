Embedding the Interpreter
=========================

Basic Components
----------------

To use the lisp interpreter, there are a few basic concepts to understand.

The interpreter has a "runtime" object associated with it. It holds some
information about garbage collection. Most functions related to the interpreter
take a pointer to a ``lisp_runtime`` as their first argument. You can initialize
a runtime with ``lisp_init()`` and once initialized, you must destroy it with
``lisp_destroy()``. Note that destroying a runtime also ends up garbage
collecting all language objects created within that runtime, so if you want to
access language objects, do it before destroying the runtime.

In order to run any code, you will need to have a global scope. This object
binds names to values, including several of the critical built in functions for
the language. You can create scopes like this: ``lisp_new(rt, type_scope)``,
where ``rt`` is your lisp runtime. The scope is managed by the runtime and
should not be freed, since the garbage collector frees it when you call
``lisp_destroy()``. Once you have a scope, you typically will want to use
``lisp_scope_populate_builtins()`` to add all the critical builtins to your
global scope.

Finally, you need to know a little bit about garbage collection. The lisp
interpreter uses a mark and sweep garbage collector. This means that every so
often the application must pause the program, mark all reachable language
objects, and free everything that is unreachable. To do this, you need a "root
set" of objects, which is typically your global scope. You should call
``lisp_mark()`` on this root set, followed by ``lisp_sweep()`` on the runtime to
free up all memory that is not reachable from your root set.

The REPL
--------

A basic REPL with libstephen lisp is fairly simple:

1. First, create a language runtime and a global scope.
2. Read a line of input.
3. Parse the input. Parsed code is simply a ``lisp_value`` like any other
   language object.
4. Evaluate the input within the global scope.
5. Print the output, and a trailing newline.
6. Mark everything in scope, then sweep unreachable objects.
7. Repeat steps 2-7 for each line of input.
8. Destroy the language runtime to finish cleaning up memory.

Here is some basic code that demonstrates embedding a simple lisp interpreter,
without any custom functions. It uses the ``editline`` implementation of the
``readline`` library for reading input (and allowing line editing).

.. code:: C

   #include <editline/readline.h>
   #include <stdio.h>

   #include "libstephen/lisp.h"

   int main(int argc, char **Argo)
   {
     // 1. Create runtime & scope
     lisp_runtime rt;
     lisp_init(&rt);
     lisp_scope *scope = (lisp_scope*)lisp_new(&rt, type_scope);
     lisp_scope_populate_builtins(&rt, scope);

     // Add your own builtins here?

     while (true) {
       // 2. Read a line of input
       char *input = readline("> ");
       if (input == NULL) {
         break;
       }
       // 3. Parse input
       lisp_value *value = lisp_parse(&rt, input);
       add_history(input); // for editline history only
       free(input);
       // 4. Evaluate input within global scope.
       lisp_value *result = lisp_eval(&rt, scope, value);
       // 5. Print output and a trailing newline.
       lisp_print(stdout, result);
       fprintf(stdout, "\n");
       // 6. Call garbage collector.
       lisp_mark(&rt, (lisp_value*)scope);
       lisp_sweep(&rt);
     }

     // 8. Destroy the language runtime.
     lisp_destroy(&rt);
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

   lisp_value *lisp_builtin_somename(lisp_runtime *rt,
                                     lisp_scope *scope,
                                     lisp_value *arglist);

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
variables. Similar to ``sscanf()``, it reads a type code from the format string
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
(``lisp_error_new()``). If it doesn't fail, your function is free to do whatever
logic you'd like.

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
- ``lisp_eval(lisp_runtime *rt, lisp_scope *s, lisp_value *v)``
- ``lisp_call(lisp_runtime *rt, lisp_scope *s, lisp_value *callable, lisp_value *arguments)``
  - invokes ``callable`` on ``arguments`` in scope ``s``.

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
       return (lisp_value*)lisp_error_new("error: expected a string!");
     }

     // Perform our logic.
     printf("Hello, %s!\n", s->s);

     // we have to return something...
     return (lisp_value*) lisp_nil_new();
   }


   int main(int argc, char **Argo)
   {
     lisp_runtime rt;
     lisp_init(&rt);
     lisp_scope *scope = (lisp_scope*)lisp_new(&rt, type_scope);
     lisp_scope_populate_builtins(&rt, scope);

     lisp_scope_add_builtin(&rt, scope, "hello", say_hello);

     while (true) {
       char *input = readline("> ");
       if (input == NULL) {
         break;
       }
       lisp_value *value = lisp_parse(&rt, input);
       add_history(input); // for editline history only
       free(input);
       lisp_value *result = lisp_eval(&rt, scope, value);
       lisp_print(stdout, result);
       fprintf(stdout, "\n");
       lisp_mark(&rt, (lisp_value*)scope);
       lisp_sweep(&rt);
     }

     lisp_destroy(&rt);
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
