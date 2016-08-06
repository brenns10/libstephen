Lisp Implementation Documentation
=================================

Included in libstephen (for some reason) is a somewhat functional lisp
implementation. This feature is intended to provide some lightweight scripting
capabilities for applications using the library, without having to link to a
more complex tool like Python or Lua.

For instance it may be possible for an application to write all its routines in
a way that they are callable from libstephen's lisp interpreter, and then to run
the main program as a small lisp script. Or an application could use the lisp
implementation as a powerful and easy to use configuration language for startup.
A final possibility might be enabling extensions and plugins for an application.

This section is organized so that the documentation required for a library user
to embed the interpreter is presented first. More advanced information about the
inner workings of the interpreter is presented later.

Contents:

.. toctree::
   :maxdepth: 2

   refcount.rst
   embedding.rst
   types.rst
