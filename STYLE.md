# `libstephen` Code Style Guide

Despite being a single developer, I haven't always been the most consistent with
my code style.  I'm writing this document to standardize myself on a standard
set of conventions.

## Naming

### Data Structures

* Data structures should be named `smb_` followed by a two to four character
  abbreviation of their name/purpose.  Examples include:
  + `smb_ll` - Linked list.
  + `smb_iter` - Iterator.
* Data structures should be declared as `typedef struct [name] { .... } [name];`
  so that they may be referenced with or without the `struct` keyword.
* Data structure member variables should be lower case,
  `underscores_between_words`, and should not have any abbreviation indicating
  their data structure.

### Functions

* Functions should be named with `underscores_between_words`.
* Functions that deal with a module or data structure should be prefixed with
  the two to four character abbreviation for that data structure or module.
* Functions that do not deal with a data structure, but instead are library
  level functions, should be prefixed with `smb_`.
* Private functions (those local to a code file, and without a declaration in a
  header) need not be prefixed with anything, but should still follow the
  `underscores_between_words` convention.

### Variables

* Local variables should also be named with `underscores_between_words`.
* Global variables should not be used enough to merit a specific style.

### Constants and Macros

* Constants and macros should be in `ALL_CAPS_UNDERSCORES_BETWEEN_WORDS`.
* They should also generally be prefixed by `SMB_`.

### Files

* All header files should go within the `libstephen/` directory.
* Library level declarations go within `libstephen/base.h`.
* Module level declarations should go within a file named after the module/data
  structure's abbreviation.
* Source files should generally be named after the module that they define.
  However, they may use an expanded version of the module's name so as to be
  clearer.

## Formatting

### Operators

* Binary and ternary operators should be surrounded by spaces, except when
  compressing them lends readability to code.
* Comparison operators should be surrounded by spaces.
* Assignment operators should be surrounded by spaces.
* Unary operators (`-`, `++`, `--`, `*`, `&`) should be attached to their
  operand.  This applies to the pointer operators too.  In declarations, the `*`
  should be attached to the variable being declared.
* The dereferencing operator `->` and field access operator `.` should not be
  surrounded by spaces.

### Braces

* For function blocks, braces should start and end on a new line, and be the
  only characters on those lines.
* For control flow structures, the opening brace should be on the same line as
  the initial keyword, and the closing brace should be on its own line.
* When braces are on their own line, they should be at the indentation level of
  the containing block, not the block they define.

### Parentheses

* Parentheses, in all cases, should not be separated from the text they enclose.
* In function calls, the opening parenthesis should not be separated from the
  function name.

### Indentation

* Every block should be indented two spaces from the containing block.
* Documentation comments should have an indentation level of three spaces from
  their containing block.

### Comments

* Both `//` and `/* ... */` comments are allowed.  However, `//` should be
  preferred for most line comments that span one or two lines.
* Larger, multiline comments can use `/* ... */`, but you should consider
  whether that comment shouldgo within a documentation comment instead of a
  regular old comment.
* Every function (public or private) should have a documentation comment `/***
  ... */` before it.  It should use the Doxygen format with `@` as the keyword
  character.  It should at least define a `@brief` for a one line explanation of
  the function's purpose, and in most cases should document all parameters,
  return values, and errors, and provide an explanation sufficient for an API
  user.
* Public functions are required to have the above.
* Files should start with a standard header (as can be seen in
  `src/linkedlist.c`).  It should contain `@file`, `@author`, `@date`, `@brief`,
  and `@copyright`.  After the `@brief`, a more full explanation of the module
  may be provided, if necessary.
* Comments may be used to delimit sections of code.  They should be of the form
  forward slash, 79 asterisks, blank line, centered header title line, blank
  line, 79 asterisks, and forward slash.

### Miscellaneous

* System headers should be included first, followed by a blank line, followed by
  libstephen headers.

## Code

### Error Handling

* Error handling should always be achieved by a `status` parameter to a
  function.  The status parameter should be the last parameter in the function,
  and it should have type `smb_status*`.
* `smb_status` should be an enumerated type, defining status codes named like
  constants:
  + `SMB_SUCCESS`
  + `SMB_INDEX_ERROR`
  + `SMB_NULL_ERROR`
  + etc
* The status variable should be checked after every function call.  No
  exceptions.
  + If an error condition is impossible, then that should be documented with an
    assertion.
* `goto`s are permissable for handling errors in code which needs to free
  resources.

### Assertions

* Assertions from `assert.h` should be used throughout code.  As with all
  assertions, they should only be used to indicate a *bug in the program*.  That
  is, assert things that could only happen if there was a bug in the program.
  An assertion should never fire due to something like out of bounds input
  (unless it is an internal function where all input has already been checked).
