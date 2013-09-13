/*******************************************************************************

  File:         generic.h

  Author:       Stephen Brennan

  Date Created: Thursday, 12 September 2013

  Description:  A generic data type used in data structures

*******************************************************************************/

union long_data {
  long long int data_llint;
  double data_dbl;
  void * data_ptr;
};

typedef union long_data DATA;
