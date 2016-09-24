.. libstephen documentation master file, created by
   sphinx-quickstart on Fri Jul 31 11:08:00 2015.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Welcome to libstephen's documentation!
======================================

Here you will find documentation on libstephen, my personal C library.  The
documentation below is split into two sections.  The first contains a brief
tutorial for setting up a project with the library, and then commentary on each
of the major modules of the library.  The second section contains API reference
for every header file in the project.

*Please Note:* Libstephen is very much my own, personal library and creation.  I
use it to experiment on how I should design, implement, and document APIs.  I
heavily use some portions of the library (for instance, unit testing) while I
rarely use some other portions (e.g. bitfields).  Sometimes I add, remove, or
modify interfaces without warning.  And since I haven't officially given a
version number beyond 0.1 for this library, there's no real guarantee for
interface stability.  All that is to say: if you want your project to go into
production, this is not the library you're looking for.  If you're just doing a
personal project and you don't mind using someone else's personal project,
you're in the right place!

Contents:

.. toctree::
   :maxdepth: 2

   tutorial/index.rst
   api/index.rst
   lisp/index.rst


Indices and tables
==================

* :ref:`genindex`
* :ref:`search`
