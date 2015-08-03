Logging
=======

.. code:: c

    #include "libstephen/log.h"

-  `Header
   (master) <https://github.com/brenns10/libstephen/blob/master/inc/libstephen/log.h>`__
-  `Implementation
   (master) <https://github.com/brenns10/libstephen/blob/master/src/log.c>`__

Logging is a very useful way to record messages about your program's
operation so you can debug it later. Since C's debugging tools are
rather difficult (encouraging so-called ``printf`` debugging), having a
sane logging setup that can be turned on or off easily is important.
Libstephen provides a very nice logging system that satisfies these
requirements. It is inspired by Python's logging system, but somewhat
simpler.

History
-------

When I first created Libstephen, I wanted a way to produce debugging
printouts and disable them when I didn't need them. I came up with a
macro (``SMB_DP``) that would expand to ``printf`` when ``DEBUG`` was
defined, or else expand to nothing. This worked well (so long as I
didn't rely on any side effects of expressions within the arguments!).
However, as I started to link to and use Libstephen within other
projects, I ran into issues with this system.

First was that if client code linked to a debug build of Libstephen, it
could get a flood of output from my own logging (that was enabled for
debugging). Second, I began to need more control over logging. There was
no control for enabling logging for just certain modules, or just
messages of a certain priority. That is what inspired me to build a
Python-like logging framework.

Goals
-----

I focused on the following development goals:

-  In the common case, the logger should be a drop-in replacement for
   ``printf``. It shouldn't require much extra typing or arguments.
-  There should be levels of "priority", from fine grained debugging
   output, to critical errors only. You should be able to set how much
   your logger shows.
-  Logging output should be able to be handled by multiple handlers
   (files). These handlers should be allowed to set their own log levels
   in addition to the logger.
-  Log messages should tell you exactly where in the code they came
   from. In Emacs' compilation mode, you should be able to hit enter on
   a log message and go directly to its source.

Concepts
--------

The logging system is made up of loggers (``smb_logger``), which are
collections of log handlers (``smb_loghandler``), along with a log
level. Log handlers have their own log level, and they have a file
associated with them. The logging system also has a single "default"
logger, which is used when you don't specify a logger. At startup, the
default logger will report any level message to ``stderr``.

The log levels are defined as macros expanding to integers:

.. code:: c

    #define LEVEL_NOTSET    0
    #define LEVEL_DEBUG    10
    #define LEVEL_INFO     20
    #define LEVEL_WARNING  30
    #define LEVEL_ERROR    40
    #define LEVEL_CRITICAL 50

You can log at any integer level, so these levels are just suggestions,
but they come with nice macros for logging.

Functions & Macros
------------------

You can create a ``smb_logger`` using the default lifetime functions
(``sl_*``), which take no extra arguments. You can use the following
functions to adjust loggers:

.. code:: c

    void sl_set_level(smb_logger *l, int level);
    void sl_add_handler(smb_logger *l, smb_loghandler h, smb_status *status);
    void sl_clear_handlers(smb_logger *l);
    void sl_set_default_logger(smb_logger *l);

Each of these functions can take ``NULL`` in order to reference the
default logger. As you can see, you can also set the default logger so
that code that logs to the default logger will use your own instead. If
you call ``sl_set_default_logger(NULL)``, the default logger should be
completely reset to defaults.

Once you have your loggers (which you don't have to do immediately,
since you could just use the default logger), you can use the ``LOG``
macro to begin logging:

::

    LOG(pointer to logger, log level, "format string", format args...)

This is a macro, so in general avoid using side effects within the
arguments. The current implementation will not evaluate expressions more
than once, but I make no guarantee about that.

If you're using the default logger, you have the following shortcuts:

::

    DDEBUG("format string", format args ...)
    DINFO("format string", format args ...)
    DWARNING("format string", format args ...)
    DERROR("format string", format args ...)
    DCRITICAL("format string", format args ...)

Think of the ``D`` as standing for Default. If you are using any other
logger, you have these shortcuts:

::

    LDEBUG(logger, "format string", format args ...)
    LINFO(logger, "format string", format args ...)
    LWARNING(logger, "format string", format args ...)
    LERROR(logger, "format string", format args ...)
    LCRITICAL(logger, "format string", format args ...)

Think of the ``L`` as standing for Logger. You have to specify your
logger.

All of these macros expand to call a single function:

.. code:: c

    void sl_log(smb_logger *l, char *file, int line, const char *function, int level, ...);

You don't want to call this function yourself, since most of those
arguments (file, line, function) are provided by preprocessor arguments.
