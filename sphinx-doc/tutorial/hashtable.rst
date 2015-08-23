Hash Table
==========

.. code:: C

    #include "libstephen/ht.h"

-  `Header
   (master) <https://github.com/brenns10/libstephen/blob/master/inc/libstephen/ht.h>`__
-  `Implementation
   (master) <https://github.com/brenns10/libstephen/blob/master/src/hashtable.c>`__

The hash table associates keys with values. It uses a "hash" to
determine the location of a key inside an array, and uses that to
achieve nearly constant time lookup. A hash table needs enough memory to
ensure that the table is about half empty, in order to work properly.
So, the table basically trades off memory for access speed.

Infrastructure
--------------

Hash tables need a good deal more infrastructure than a simple list
needs. A list can store values without knowing or caring about their
type. On the other hand, a hash table needs to know the data type of its
keys for these reasons: \* Hashing! A hash function takes a key and
produces a number, which can hopefully be used to index into the table.
You need to know the data type in order to come up with a good hash
table. \* Equality testing! A hash function is bound to have collisions.
So, a hash table needs to be able to check whether keys are equal, and
that means knowing the type of the keys.

To handle this, the hash table takes function pointers of two different
types. The first is called ``HASH_FUNCTION``:

.. code:: C

    typedef unsigned int (*HASH_FUNCTION)(DATA toHash);
    unsigned int my_hash(DATA toHash) {
      //compute hash value
      return hash_value;
    }

That ``typedef`` is a bit cryptic, but it is a function pointer to a
function that takes a ``DATA`` and returns an ``unsigned int``. The
function below that is a hash function that implements the
``HASH_FUNCTION`` signature. The only hash functions in libstephen are
``ht_string_hash`` for strings.

The other type of function pointer that is defined is ``DATA_COMPARE``:

.. code:: C

    typedef int (*DATA_COMPARE)(DATA d1, DATA d2);
    int compare_int(DATA d1, DATA d2)
    {
      return d1.data_llint - d2.data_llint;
    }

This function compares two ``DATA``. It returns 0 if they are equal. It
returns ``> 0`` if ``d1 > d2``, and ``< 0`` if ``d1 < d2``. These can
also be used for sorting, if I decide to implement it. Comparisons are
implemented for integers, doubles, pointer values, and strings.

One other function pointer is declared, in order to make it easier for
users to free values from the table. If you wanted to remove a (key,
value) pair from the table, where the value was a pointer to a data
structure, chances are you'd need to free it first. So, you'd end up
looking up the value, freeing it, then removing it from the table.
Instead, I've made a function pointer type that will act on a DATA,
allowing you to specify an action to happen before removing an item from
the table!

.. code:: C

    typedef void (*DATA_ACTION)(DATA d);
    void freer(DATA d)
    {
      free(d.data_ptr);
    }

Operations
----------

With those function pointer types in hand, we can do a lot of fun hash
table stuff. Here is a list of operations, straight from the header:

.. code:: C

    void ht_init(smb_ht *pTable, HASH_FUNCTION hash_func, DATA_COMPARE equal);
    smb_ht *ht_create(HASH_FUNCTION hash_func, DATA_COMPARE equal);
    void ht_destroy_act(smb_ht *pTable, DATA_ACTION deleter);
    void ht_destroy(smb_ht *pTable);
    void ht_delete_act(smb_ht *pTable, DATA_ACTION deleter);
    void ht_delete(smb_ht *pTable);

    void ht_insert(smb_ht *pTable, DATA dKey, DATA dValue);
    void ht_remove_act(smb_ht *pTable, DATA dKey, DATA_ACTION deleter,
                       smb_status *status);
    void ht_remove(smb_ht *pTable, DATA dKey, smb_status *status);
    DATA ht_get(smb_ht const *pTable, DATA dKey, smb_status *status);
    void ht_print(smb_ht const *pTable, int full_mode);

I've included the ``init``/``create``/``delete``/``destroy`` functions,
because the ``delete`` and ``destroy`` ones have ``_act`` variants that
apply an action to every value in the table, before destroying the
table.

We have insertion, removal, and retrieval. There's also a printing
function, which is really more of a debugging print function. It reveals
some of the structure of the hash table, which is good for debugging.

Sample Usage
------------

Here's an example of a using a hash table:

.. code:: C

    DATA d1, d2;
    smb_status status = SMB_SUCCESS;
    smb_ht *ht = ht_create(&ht_string_hash, &data_compare_string);

    d1.data_ptr = "stephen";
    d2.data_ptr = "brennan";
    ht_insert(ht, d1, d2);
    d2 = ht_get(ht, d1, &status);
    assert(status == SMB_SUCCESS);

    printf("%s: %s\n", d1.data_ptr, d2.data_ptr);
    //STDOUT: stephen: brennan

Structure
---------

The hash table structure I use could definitely be improved, and I may
improve it in the future. Currently, it uses the simplest possible
method for resolving collisions: chaining. Basically, each bucket is a
linked list. In the future, I might change to quadratic probing. Anyhow,
here is the hash table itself:

.. code:: C

    typedef struct smb_ht
    {
      int length;
      int allocated;
      HASH_FUNCTION hash;
      DATA_COMPARE equal;
      struct smb_ht_bckt **table;
    } smb_ht;

And here is the structure of the hash table bucket:

.. code:: C

    typedef struct smb_ht_bckt
    {
      DATA key;
      DATA value;
      struct smb_ht_bckt *next;
    } smb_ht_bckt;
