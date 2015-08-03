Regular Expressions
===================

.. code:: c

    #include "libstephen/regex.h"
    #include "libstephen/fsm.h"

-  `Header - regex
   (master) <https://github.com/brenns10/libstephen/blob/master/inc/libstephen/regex.h>`__
-  `Header - fsm
   (master) <https://github.com/brenns10/libstephen/blob/master/inc/libstephen/fsm.h>`__
-  `Implementation - regex
   (master) <https://github.com/brenns10/libstephen/blob/master/src/regex/>`__
-  `Implementation - fsm
   (master) <https://github.com/brenns10/libstephen/blob/master/src/fsm/>`__

Regular expressions are a pretty powerful tool for text processing.
POSIX defines ``regcomp()``, but it's not standard C or portable. Plus,
I wanted to implement regular expressions myself for my CKY parser
project. So, I did, and included them in my library.

Features
--------

I'm not going to explain how regular expressions work here, but I will
specify what features my regular expression engine has:

-  Character classes:

   -  Specify character ranges with a hyphen.
   -  Negate them with a ``^``
   -  Use a hyphen in a character class by putting it at the end

-  Special character classes: ``\d``, ``\w``, ``\s``, etc.
-  Sub-expressions: ``(sub-expression here)``
-  Logical OR using ``|`` bar.
-  Kleene star ``*``
-  At least one ``+``
-  Zero or one ``?``
-  Backslash escaping
-  Capture groups: ``(?expression to be captured here)``

A few notes on capturing:

-  Be careful with capture groups. They're newly implemented, and
   probably very buggy. For instance, I'm not sure how something like
   this: ``(?\d+)+`` would work, let alone ``(?\d*)*``. Make sure you
   place adjacent capture groups in a way that you can always tell them
   apart, so the engine doesn't get "confused".
-  **Don't do nested captures!** My engine *definitely* doesn't support
   this. While the matching shouldn't crash and burn, the capture groups
   returned will almost certainly be junk.

Functions
---------

Here is the main interface for compiling regular expressions:

.. code:: c

    fsm *regex_parse(const wchar_t *regex);

I can almost guarantee that this interface will change. I spent a lot of
time improving parsing in CKY (when the regex implementation was still
part of that project). However, the regex parsing functions are still
rather rough around the edges. Chances are you won't get an error
message when you enter a bad regex. You'll probably just get a segfault
or an infinite loop. When I improve the regex parse implementation, I'll
probably include some error handling.

Anyway, the parse function returns an instance of ``fsm``, which is a
data structure that represents a finite state machine. The theory isn't
really relevant, but the short of it is this: a FSM is a theoretical
machine that can decide to accept or reject an input string. You can
find out if a string is accepted or rejected by simulating the FSM on
it. The following functions are provided for that purpose:

.. code:: c

    bool fsm_sim_det(fsm *f, const wchar_t *input);
    fsm_sim *fsm_sim_nondet_begin(fsm *f);
    int fsm_sim_nondet_state(const fsm_sim *s, wchar_t input);
    void fsm_sim_nondet_step(fsm_sim *s, wchar_t input);
    smb_al *fsm_sim_get_captures(fsm_sim *sim);
    bool fsm_sim_nondet(fsm *f, const wchar_t *input);
    bool fsm_sim_nondet_capture(fsm *f, const wchar_t *input, smb_al **capture);

Any ``fsm`` returned by the ``regex_parse()`` function is going to be
very nondeterministic. This means you'll have to use
``fsm_sim_nondet()`` or its siblings. ``fsm_sim_nondet()`` can tell you
whether a string matches. If you want to search for an occurrence of a
pattern in a search text, you'd need to get your hands a bit dirtier and
use the sub-functions of ``fsm_sim_nondet``. The functions
``regex_search()`` or ``fsm_search()`` (declared in ``regex.h``,
implemented in ``src/regex/search.c``) may teach you a bit about how you
would do such searches, or you can just use one of them (they will find
all occurrences of a pattern in a search text).

If you're looking to get your captured groups, you'll have to go with
``fsm_sim_nondet_capture()``, which takes an out-parameter which gets
assigned a pointer to a list of integers (the start/stop indices of the
groups). It will return NULL for 0 captures, and it'll do nothing if the
simulation is rejected or if you provide NULL as the ``capture``
argument.
