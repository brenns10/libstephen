Type System
===========

As previous pages have mentions, lisp objects rely on a mark and sweep garbage
collection system to manage their lifetimes. This page describes how the type
system is implemented, which includes the garbage collection implementation.

In Java, everything is an object. In this language, everything is a
``lisp_value``.  This means two things:

1. Every object contains a ``type`` pointer.
2. Every object has a variable to store its ``mark``.
3. Every object has a pointer to the object allocated after it, forming a linked
   list.

Every type declares these using the ``LISP_VALUE_HEAD`` macro, like so:

.. code:: C

   typedef struct {
     LISP_VALUE_HEAD;
     int x;
   } lisp_integer;

You can cast pointers to these objects to ``lisp_value*`` and still access the
type object. All objects are passed around as ``lisp_value*``.

In order to allow objects to be treated differently based on their type (but in
a generic way to calling code), we use the type object.

A type object is just another object, with type ``lisp_type``. However, it is
NOT managed by the garbage collector. It contains the string name of a type,
along with pointers to implementations for the following functions: print, new,
free, eval, call, and expand. Therefore, if you have ``lisp_value *object`` and
you want to print it, you can do:

.. code:: C

   object->type->print(stdout, object);

Unfortunately that's very verbose. To simplify, each of the functions
implemented by a type object has an associated helper function. So you can
instead do:

.. code:: C

   lisp_print(stdout, object);

But there's no magic or switch statements involved here--we're simply using the
type object.

This means that it's not too difficult to add a type to the language! All you
need to do is declare a struct for your type, implement the basic functions, and
create a type object for it. Here is an example for a regex type built on
libstephen's regex implementation:

.. code:: C

   // HEADER
   typedef struct {
     LISP_VALUE_HEAD;
     Regex r;
   } lisp_regex;
   extern lisp_type *type_regex;

   // CODE
   static void regex_print(FILE *f, lisp_value *v);
   static lisp_value *regex_new(void);
   static void lisp_value_free(void *v);
   static lisp_value *regex_eval(lisp_runtime *rt, lisp_scope *s, lisp_value *re);
   static lisp_value *regex_call(lisp_runtime *rt, lisp_scope *s, lisp_value *c, lisp_value *a);
   static smb_iter regex_expand(lisp_value *v);

   static lisp_type *type_regex_obj = {
     .type=type_type,
     .name="regex",
     .print=regex_print,
     .new=regex_new,
     .free=regex_free,
     .call=regex_call,
     .expand=regex_expand,
   };
   lisp_type *type_regex = &type_regex_obj;

   // function implementations

Many function implementations in the type object are trivial. However, a few are
rather important to get right:

- The print function should not print a newline.
- The new function should take no arguments. It does not need to bother to
  populate the ``lisp_value`` fields such as ``type`` or ``next``. Instead,
  these will be called by the ``lisp_new()`` wrapper function. It may be wise to
  write a ``lisp_new_TYPENAME(args...)`` function so users can easily create an
  instance of a type from some arguments.
- The eval function is tricky. Many things just don't get evaluated. For
  instance, a regex object would probably be returned by a builtin function, but
  never produced as raw code to be evaluated. So its eval function should
  probably return an error.
- Generally the call function should return an error. However it may not be the
  worst thing to make creative use of function call syntax. For instance, it
  would be interesting to have a regex type that, when called, matches on a
  string and returns the match object.
- The most critical function to implement correctly is expand. This must return
  a generator, which yields pointers to other ``lisp_value`` objects contained
  by this object. For simple objects, this will be trivial, but for data
  structures it is incredibly important to get right.
