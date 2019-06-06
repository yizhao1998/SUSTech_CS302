/* Spawns 10 child processes, all of which read from the same
   file and make sure that the contents are what they should
   be. */

#include <random.h>
#include <stdio.h>
#include <syscall.h>
#include "tests/lib.h"
#include "tests/main.h"
#include "tests/filesys/base/syn-read.h"

static char buf[BUF_SIZE];

#define CHILD_CNT 10

void
test_main (void) 
{
  pid_t children[CHILD_CNT];
  int fd;

  // printf ("1\n");
  CHECK (create (file_name, sizeof buf), "create \"%s\"", file_name);
  // printf ("2\n");
  CHECK ((fd = open (file_name)) > 1, "open \"%s\"", file_name);
  // printf ("3\n");
  random_bytes (buf, sizeof buf);
  CHECK (write (fd, buf, sizeof buf) > 0, "write \"%s\"", file_name);
  msg ("close \"%s\"", file_name);
  close (fd);

  exec_children ("child-syn-read", children, CHILD_CNT);
  wait_children (children, CHILD_CNT);
}
