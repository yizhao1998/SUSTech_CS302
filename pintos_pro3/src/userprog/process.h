#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"
#include "filesys/file.h"
struct file_control_block
{
  struct file *process_file;
  int fd;
  struct list_elem file_elem;
};
tid_t process_execute (const char *file_name);
int process_wait (tid_t);
void process_exit (void);
void process_activate (void);
int add_file (struct file *f);
struct file * get_file (int fd);
bool close_file (int fd);

#endif /* userprog/process.h */
