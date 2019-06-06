#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "lib/kernel/stdio.h"
#include "devices/shutdown.h"
#include "filesys/filesys.h"
#include "process.h"
#include "pagedir.h"
#include "filesys/file.h"
#ifndef ARGLEN
#define ARGLEN 5
#endif

typedef int pid_t;
int success_flag = 0;

static void syscall_handler (struct intr_frame *);
static void is_valid_ptr (const void *ptr);
static void is_valid_ptr_single (const void *ptr);
void halt (void) NO_RETURN;
void exit (int status) NO_RETURN;
pid_t exec (const char *file);
int wait (pid_t);
bool create (const char *file, unsigned initial_size);
bool remove (const char *file);
int open (const char *file);
int filesize (int fd);
int read (int fd, void *buffer, unsigned length);
int write (int fd, const void *buffer, unsigned length);
void seek (int fd, unsigned position);
unsigned tell (int fd);
void close (int fd);
bool is_in_valid_page (const void * ptr);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  // Check if f->esp is a valid pointer)
  is_valid_ptr (f->esp);
  // Cast f->esp into an int *, then dereference it for the SYS_CODE

  int *p = f->esp;
  int argv[ARGLEN];  
  switch(*p)
  {
    case SYS_HALT:
    {
      halt ();
      break;
    }
    case SYS_EXIT:
    {
      read_args (1, argv, f);
      exit (argv[0]);
      break;
    }
    case SYS_EXEC:
    {
      read_args (1, argv, f);
      is_valid_ptr ((void *) *(++p));
      f->eax = exec (argv[0]);
      break;
    }
    case SYS_WAIT:
    {
      read_args (1, argv, f);
      f->eax = wait (argv[0]);
      break;
    }
    case SYS_CREATE:
    {
      read_args (2, argv, f);
      f->eax = create (argv[0], argv[1]);
      break;
    }
    case SYS_REMOVE:
    {
      read_args (1, argv, f);
      f->eax = remove (argv[0]);
      break;
    }
    case SYS_OPEN:
    {
      read_args (1, argv, f);
      f->eax = open (argv[0]);
      break;
    }    
    case SYS_FILESIZE:
    {
      read_args (1, argv, f);
      f->eax = filesize (argv[0]);
      break;
    }
    case SYS_READ:
    {
      read_args (3, argv, f);
      f->eax = read (argv[0], argv[1], argv[2]);
      break;
    }
    case SYS_WRITE:
    {
      read_args (3, argv, f);
      f->eax = write (argv[0], argv[1], argv[2]);
      break;
    }
    case SYS_SEEK:
    {
      read_args (2, argv, f);
      seek(argv[0], argv[1]);
      break;
    }
    case SYS_TELL:
    {
      read_args (1, argv, f);
      f->eax = tell (argv[0]);
      break;
    }
    case SYS_CLOSE:
    {
      read_args (1, argv, f);
      close (argv[0]);
      break;
    }
  }
}

/* Get all arguments and check them. */
void 
read_args (int n, int *argv, struct intr_frame *f)
{
  int i;
  int *ptr;
  for (i = 0; i < n; ++i) 
  {
    ptr = ((int *)f->esp + 1 + i);
    is_valid_ptr ((void *)ptr);
    argv[i] = *ptr;
  }
}

/* Check a pointer completely. */
static void 
is_valid_ptr (const void *ptr)
{
  is_valid_ptr_single ((int *)ptr);
  for (int i = 0; i < 4; ++i)
    is_valid_ptr_single ((char *)ptr + i);
}

/* Check a pointer's address and its page. */
static void 
is_valid_ptr_single (const void *ptr)
{
  if (ptr == NULL || !is_user_vaddr (ptr) || !is_user_vaddr_above (ptr))
    exit (-1);
  void *ptr_check = pagedir_get_page (thread_current ()->pagedir, ptr);
  if (!ptr_check)
    exit (-1);
}

void 
halt ()
{
  shutdown_power_off ();
}

void 
exit (int status)
{
  struct thread *cur = thread_current ();
  struct thread *p = thread_find (cur->parentId);
  /* Change the corresponding child_process in parent's children list. */
  if (p != NULL)
  {
    struct list_elem *e;
    struct child_process *child;
    for (e = list_begin (&p->children); e != list_tail (&p->children); e = list_next (e))
    {
      child = list_entry (e, struct child_process, childelem);
      if (child->tid == cur->tid)
      {
        lock_acquire (&cur->child_lock);
        child->exited = true;
        child->exit_status = status;
        lock_release (&cur->child_lock);
        break;
      }
    }
  }
  printf ("%s: exit(%d)\n", cur->name, status);
  thread_exit ();
}

pid_t 
exec (const char *file)
{
  if (!file)
    exit (-1);
  /* Check the file pointer. */
  is_valid_ptr (file);
  pid_t pid = -1;
  pid = process_execute (file);
  return pid;
}

int 
wait (pid_t child_tid)
{
  int res = 0;
  res = process_wait (child_tid);
  return res;
}

bool 
create (const char *file, unsigned initial_size)
{
  if (!file)
    exit (-1);
  /* Check the file pointer. */
  is_valid_ptr (file);
  bool flag;
  /* Synchronization safety. */
  lock_acquire (&filesys_lock);
  flag = filesys_create (file, initial_size);
  lock_release (&filesys_lock);
  return flag;
}

bool 
remove (const char *file)
{
  bool flag;
  /* Check the file pointer. */
  is_valid_ptr (file);
  lock_acquire (&filesys_lock);
  flag = filesys_remove (file);
  lock_release (&filesys_lock);
  return flag;
}

int 
open (const char *file)
{
  if (!file)
    exit (-1);
  /* Check the file pointer. */
  is_valid_ptr (file);
  
  lock_acquire (&filesys_lock);
  struct file *f = filesys_open (file);
  if (!strcmp (file, thread_current ()->name))
    file_deny_write (f);
    
  if (!f)
  {
    lock_release (&filesys_lock);
    return -1;
  }
  int fd = add_file (f);
  lock_release (&filesys_lock);

  return fd;
}

int 
filesize (int fd)
{
  lock_acquire (&filesys_lock);
  struct file *f = get_file (fd);
  if (!f)
  {
    lock_release (&filesys_lock);
    return -1;
  }
  int size = file_length (f);
  lock_release (&filesys_lock);
  return size;
}

int 
read (int fd, void *buffer, unsigned length)
{
  if (!buffer)
    exit (-1);
  /* Check the buffer pointer. */
  is_valid_ptr (buffer);

  if (fd == STDOUT_FILENO)
    return length;
  
  lock_acquire (&filesys_lock);
  struct file *f = get_file (fd);
  if (!f)
  {
    lock_release (&filesys_lock);
    return -1;
  }
  int size = file_read (f, buffer, length);
  lock_release (&filesys_lock);

  return size;
}

int 
write (int fd, const void *buffer, unsigned size)
{
  if (!buffer)
    exit (-1);
  
  /* Check all the buffer pointers. */
  for (int i = 0; i < size; i++) 
    is_valid_ptr (buffer + i);
  
  if (fd == STDOUT_FILENO)
  {
    putbuf (buffer, size);
    return size;
  }
  if (fd == STDIN_FILENO)
    return -1;

  lock_acquire (&filesys_lock);
  struct file *f = get_file (fd);
  if (!f)
  {
    lock_release(&filesys_lock);
    return -1;
  }
  /* If a file cannot be written. */
  if (is_deny_write(f))
  {
    lock_release (&filesys_lock);
    return 0;
  }
  int length = file_write (f, buffer, size);
  lock_release (&filesys_lock);

  return length;
}

/* Move the pointer to the correct position in a file. */
void 
seek (int fd, unsigned position) 
{
  lock_acquire (&filesys_lock);
  struct File *file = get_file (fd);
  if (!file) 
  {
    lock_release (&filesys_lock);
    return -1;
  }
  file_seek (file, position);
  lock_release (&filesys_lock);
}

/* Get the correct position in a file. */
unsigned 
tell (int fd) 
{
  lock_acquire (&filesys_lock);
  struct File *file = get_file (fd);
  if (!file)
  {
    lock_release (&filesys_lock);
    return -1;
  }
  off_t size = file_tell (file);
  lock_release(&filesys_lock);
  return size;
}

void 
close (int fd) 
{
  lock_acquire (&filesys_lock);
  bool res = close_file (fd);
  if (!res)
  {
    lock_release (&filesys_lock);
    exit(-1);
  }
  lock_release (&filesys_lock);
}


