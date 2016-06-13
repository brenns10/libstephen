Regular Expressions
===================

.. code:: C

    #include "libstephen/re.h"
    #include "libstephen/re_internals.h"

-  `Header - re
   (master) <https://github.com/brenns10/libstephen/blob/master/inc/libstephen/re.h>`__
-  `Header - re_internals
   (master) <https://github.com/brenns10/libstephen/blob/master/inc/libstephen/re_internals.h>`__
-  `Implementation - regex
   (master) <https://github.com/brenns10/libstephen/blob/master/src/regex/>`__

Regular expressions are a pretty powerful tool for text processing.  They allow
you to match or search for a certain type of text patterns.  POSIX defines
``regcomp()``, but that's a limited language, and it's not in the C standard.
Since I'm very interested in regular expressions, I implemented them myself.  My
current implementation is based on a series of `articles
<https://swtch.com/~rsc/regexp/>`_ by `Russ Cox <https://swtch.com/~rsc/>`_.

This first section of this document explains basic regular expression syntax.
The next section discusses how to use my implementation.  The final section
gives a discussion on the inner workings of my implementation.

Syntax
------

Regular expressions are patterns that match text.  The basic regular expression
is a single character, which matches itself.  If you append a ``*``, the
expression matches 0 or more instances of that character.  If you append a
``?``, the expression matches 0 or 1, and if you append a ``+``, the expression
matches 1 or more.  You may also append a ``?`` to any of those repetition
operators in order to make them "non-greedy" - that is, they will consume as few
characters as possible.

If you have two regular expressions, A and B, you can concatenate them (AB) so
they will match the concatenated matches of A and B.  For example, ``a`` matches
``a``, ``b`` matches ``b``, and ``ab`` matches ``ab``.  You can also combine
regular expressions A and B with the pipe operator (``a|b``), which will match
either the first, or the second expression.  Finally, you can group regular
expressions using parentheses.  Text matched within parentheses will be
"captured."

One other feature: character classes.  Imagine you want to match all lowercase
letters.  Then, you would have to write ``(a|b|c|....|x|y|z)`` which would get
really long and annoying.  So, character classes give you the ability to define
whole sets of characters to match.  To define a character class, use brackets.
So, you could do this instead: ``[abc...xyz]``.  While this saved us 25
characters, it's not much of an improvement.  You can also define a range of
characters, like so: ``[a-z]``.  That's even better.  If you'd like to match a
hyphen, you can put it at the end of the class, or escape it with a backslash.
You can also negate the character class with a caret: ``[^a-z]``.  Special
character classes are:

- ``\w`` matches word characters: ``[a-zA-Z0-9_]``.  ``\W`` negates it.
- ``\d`` matches digits: ``[0-9]``.  ``\D`` negates it.
- ``\s`` matches whitespace: ``[ \n\r\t\v\f]``.  ``\S`` negates it.

You also can backslash escape any metacharacter to use it like normal, and you
can also backslash escape many whitespace characters.

The operator precedence is as follows: repetition, concatenation, alternation.
If you would like more information about regular expression grammar or
tokenization, see the discussion of the implementation details in the last
section.

Usage
-----

A regular expression first needs to be compiled before it can be used.  To do
this, use the ``recomp()`` function.  Its signature is:

.. code:: C

   Regex recomp(char *regex)

``Regex`` is the type returned from ``recomp``.  It is a small struct containing
a pointer to a buffer of compiled instructions, as well as the instruction
count.  So, you probably don't need to ever pass around pointers to these
objects, since they're already pointers.  When you're done with a regex, free it
with ``free_prog()``.

If you want to use a regex, use the ``reexec()`` function.  Here is its call
signature:

.. code:: C

   ssize_t reexec(Regex r, char *input, size_t **saved);

The first parameter is the regex returned by ``recomp()``.  The second is the
text you'd like to execute the regex on.  The function returns the length of the
string match.  It returns -1 if there is no match at all.  The final parameter
is a pointer to a pointer to the indices of the captures.  The function will set
that pointer (if non-NULL) on return.

To understand how captures work, let's look at a phone number regex:
``\(?(\d\d\d)\)?[ -]?(\d\d\d)[ -]?(\d\d\d\d)``.  This will match phone numbers
like ``(123) 456-7890``, ``1234567890``, ``123-456-7890``, or ``123 456 7890``,
as well as some odd ones like ``123)-4567890``.  In addition, it will capture
the area code, first three, and last four digits.  When you run the regex on
``(123) 456-7890``, the indices will be: ``{1, 4, 6, 9, 10, 14}``.  They are
organized in pairs.  The first index in the pair is the start index of the
captured text, and the second in the pair is the index of the first character
not included in tho capture.  The order of the pairs is determined by the
opening parenthesis in the regular expression.

Please note that you'll need to free the capture buffer.  If you're not
interested in captures, you can just set the third parameter to NULL.  If you
want to know how many indices are in the buffer, you can call ``numsaves()`` on
your regex.

There are also functions for writing regex bytecode to a textual "assembly"
representation.  This text representation can be read back in as well.  It's
actually pretty neat.  You can think of this as an implementation detail: not
something to rely on, but very useful for debugging and learning about the regex
implementation.  Here are the function signatures:

.. code:: C

   Regex read_prog(char *str);
   Regex fread_prog(FILE *f);
   void write_prog(Regex r, FILE *f);

Here is a complete example of a program that takes a regex as its first argument
and tests it on the remaining ones.

.. code:: C

   #include <stdlib.h>
   #include <stdio.h>

   #include "libstephen/re.h"


   int main(int argc, char **argv)
   {
     if (argc < 3) {
       fprintf(stderr, "too few arguments\n");
       fprintf(stderr, "usage: %s REGEXP string1 [string2 [...]]\n", argv[0]);
       exit(EXIT_FAILURE);
     }

     // Try to open first arg as file.
     Regex code;
     FILE *in = fopen(argv[1], "r");

     if (in == NULL) {
       // If it doesn't open, it's a regex we should compile.
       printf(";; Regex: \"%s\"\n\n", argv[1]);
       code = recomp(argv[1]);
       printf(";; BEGIN GENERATED CODE:\n");
     } else {
       // Otherwise, open it and read the code from it.
       code = fread_prog(in);
       printf(";; BEGIN READ CODE:\n");
     }
     write_prog(code, stdout);

     int ns = numsaves(code);
     printf(";; BEGIN TEST RUNS:\n");

     for (int i = 2; i < argc; i++) {
       size_t *saves = NULL;
       ssize_t match = reexec(code, argv[i], &saves);
       if (match != -1) {
         // It matches, report the captured groups.
         printf(";; \"%s\": match(%zd) ", argv[i], match);
         for (size_t j = 0; j < ns; j += 2) {
           printf("(%zd, %zd) ", saves[j], saves[j+1]);
         }
         printf("\n");
       } else {
         // Otherwise, report no match.
         printf(";; \"%s\": no match\n", argv[i]);
       }
     }

     free_prog(code, n);
   }

Implementation
--------------

**Grammar:**

.. code::

   REGEX (-)-> SUB
         (-)-> SUB | REGEX

   SUB   (-)-> EXPR
         (-)-> EXPR SUB

   EXPR  (-)-> TERM
         (-)-> TERM +
         (-)-> TERM + ?
         (-)-> TERM *
         (-)-> TERM * ?
         (-)-> TERM ?
         (-)-> TERM ? ?

   TERM  (1)-> char <OR> . <OR> - <OR> ^ <OR> special
         (2)-> ( REGEX )
         (3)-> [ CLASS ]
         (4)-> [ ^ CLASS ]

   CLASS (1)-> CCHAR - CCHAR CLASS
         (2)-> CCHAR - CCHAR
         (3)-> CCHAR CLASS
         (4)-> CCHAR
         (5)-> -

   CCHAR (-)-> char <or> . <OR> ( <OR> ) <OR> + <OR> * <OR> ? <OR> |

The terminal symbols of the grammar are meta-characters: ``( ) [ ] + - * ? ^
|``.  There is also ``char`` token, which represents any other character.
Backslash escaped metacharacters are also ``char`` nonterminals, as well as
backslash escaped whitespace characters.  Finally, any other backslash escaped
character is interpreted as a ``special`` terminal, which is used for things
like special character classes.

In the above grammar, some non-terminals have numbered productions, which are
stored within the parse tree itself to make it easier to generate code after the fact.

**Lexer**

The lexer generates the tokens described above.  It has a small buffer for
putting tokens back into the lexer, which just makes the parser's job a little
easier.

**Parser**

The parser is just a recursive descent parser custom written for this grammar.
The way this works is that there's a function for each nonterminal symbol, and
it has cases for each production.  These functions are co-recursive, so they
call each other whenever they need to parse another non-terminal, and not just a
terminal symbol from the lexer.  The parser generates a parse tree (the data
structure is ``PTree``).  This is used for code generation.

**Code Generation**

The code generator is also a mutually-recursive set of functions.  These go down
the parse tree, generating code as they go.  I'd love to go into how I generate
VM bytecode for each production, but I think for that you'll just have to read
through the code.  The idea behind the generation of all the code is based on
`Thompson's Construction Algorithm
<https://en.wikipedia.org/wiki/Thompson%27s_construction>`_, except for bytecode
instead of NDFA fragments.

**Virtual Machine**

The code generation is for a virtual machine based on the following ideas.

- The machine has a string pointer and a program counter.  The string pointer
  points to the current input character from the string, and the program counter
  points at the current instruction.
- Each instruction manipulates these two values.  Instructions may also
  terminate the execution signalling success or failure, and they can also
  create new "threads" - new instances of the VM and program.
- The VM also has "slots" for saving values of the SP.  This is useful for
  capturing text.

Here are the instructions of this VM:

- ``char C``: if the string pointer matches C, increment SP and PC.  Otherwise,
  fail.
- ``range A B C D``: if the SP is contained within the ranges A-B or C-D,
  increment SP and PC.  Otherwise, fail.  This instruction may have arbitrarily
  many ranges.
- ``nrange A B C D``: similar to the above, but instead it fails when the input is
  within the ranges, and continues when the input is not within the ranges.
- ``any``: increments SP and PC (so long as the SP isn't at the NUL byte).
- ``jump LABEL``: sets the PC to LABEL
- ``split L1 L2``: sets the current thread's PC to L1, and creates a new thread
  with same SP, and PC equal to L2.
- ``match``: terminate execution, reporting success.
- ``save X``: save the current value of the SP in slot X.

Given this framework, executing a program is all about running all of the
threads, and selecting the one that terminated successfully with the largest
value of the SP.  There are a number of ways to do this, but my implementation
(modeled after Rob Pike's VM, as written by Russ Cox) takes advantage of a few things:

- You can execute the threads in "lockstep", where you run each thread up to the
  point where it consumes an input from the SP.
- If you do this, you can place an upper bound on the number of threads you'll
  have at each step: it should be no more than the number of instructions in
  your program.

There's a lot going on behind the scenes, and giving an overview is tough.
Check Russ Cox's `article <https://swtch.com/~rsc/regexp/regexp2.html>`_ for a
more thorough description of the virtual machine approach.  You'll find the
approach I use under "Pike's Implementation".
