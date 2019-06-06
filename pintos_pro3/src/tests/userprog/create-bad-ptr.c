/* Passes a bad pointer to the create system call,
   which must cause the process to be terminated with exit code
   -1. */

#include "tests/lib.h"
#include "tests/main.h"

void
test_main (void) 
{
  // char *p = (char*)0x20101234;
  // printf (" start \n");
  // *p = 'a';
  // printf ("%c\n", *p);
  msg ("create(0x20101234): %d", create ((char *) 0x20101234, 0));
}
