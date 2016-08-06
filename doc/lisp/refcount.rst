Intro to Reference Counting
===========================

In order to use the libstephen lisp implementation in your code, you need to
have a basic understanding of the reference counting garbage collection system
it uses. The principles are simple, but if you don't follow them, the
interpreter will have a memory leak, or access freed memory.

Every object in the lisp interpreter has a reference count associated with it.
The count is incremented for every data structure or piece of code that owns a
reference to the object. It is decremented whenever a reference is no longer
wanted. When the reference count drops to zero, the object is freed, and any
references it owns are decremented as well.

To decrement a reference count, use the ``lisp_decref()`` function. Similarly,
to increment a reference count, use the ``lisp_incref()`` function. The question
is, when should you do these things?

1. Any object A that contains a pointer to another object B should "own" the
   reference to B. This includes lists and (especially) scope objects, which own
   references to both to the symbols that serve as the keys and the objects that
   are values.
2. Arguments to functions are "borrowed" references. They do not own the
   references and should not decrement their reference count. However, if they
   would like to maintain their own reference (for instance, by adding an object
   to a scope or list), they should increment the reference count.
3. Whenever ``lisp_eval()`` is called, it always returns a new reference (which
   the caller assumes ownership of). When the caller is done with its value, it
   should decref it accordingly.
4. Similarly, functions which can be called from the interpreter (builtins) must
   return a new reference to an object. This is because function calls are a
   part of the evaluation process. For instance, if you wrote the ``quote``
   function (which returns its first argument un-evaluated), it would look
   something like this:

   .. code:: C

      lisp_value *lisp_builtin_identity(lisp_scope *scope, lisp_value *arglist)
      {
        return lisp_incref(arglist->left);
      }

   As you can see, this function returns a *new* reference to the same object.
5. Most utility functions do not return new references (except those that create
   new objects). On exception is ``lisp_eval_list()``, which evaluates each
   element of a list. This returns a new list which owns a reference to each
   evaluated item.

The simple principles are that data structures (especially scopes) own
references. Your code generally owns references that are returned by functions
to you, and generally borrows references to its arguments.
