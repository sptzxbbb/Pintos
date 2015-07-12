#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"

static void syscall_handler (struct intr_frame *);
static int get_four_bytes_user(const void *);
static int get_user (const uint8_t *uaddr);
static bool put_user (uint8_t *udst, uint8_t byte);
void halt (void);
void exit (int status);
pid_t exec (const char*cmd_line);
int wait (pid_t pid);
bool create (const char *file, unsigned initial_size);
bool remove (const char *file);
int open (const char *file);
int filesize (int fd);
int read (int fd, void *buffer, unsigned size);
int write (int fd, const void *buffer, unsigned size);
void seek (int fd, unsigned position);
unsigned tell (int fd);
void close (int fd);

void
syscall_init (void)
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f)
{
  printf ("system call!\n");
  int call_num = get_four_bytes_user (f->esp);

  switch (call_num) {
  case SYS_HALT:
      halt ();
      NOT_REACHED();
  case SYS_EXIT:
      exit (get_four_bytes_user(f->esp + 4));
      NOT_REACHED();
  case SYS_EXEC:
      f->eax = (uint32_t) exec((const char*) get_four_bytes_user(f->esp + 4));
      break;
  case SYS_WAIT:
      break;
  case SYS_CREATE:
      break;
  case SYS_REMOVE:
      break;
  case SYS_OPEN:
      break;
  case SYS_FILESIZE:
      break;
  case SYS_READ:
      break;
  case SYS_WRITE:
      break;
  case SYS_SEEK:
      break;
  case SYS_TELL:
      break;
  case SYS_CLOSE:
      break;

  default:
      printf ("Unimplemented syatem call number");
      thread_exit();
      break;
  }

}

/* Reads a byte at user virtual address UADDR.
   UADDR must be below PHYS_BASE.
   Returns the byte value if successful, -1 if a segfault
   occurred. */
static int
get_user (const uint8_t *uaddr)
{
  int result;
  asm ("movl $1f, %0; movzbl %1, %0; 1:"
       : "=&a" (result) : "m" (*uaddr));
  return result;
}

/* Writes BYTE to user address UDST.
   UDST must be below PHYS_BASE.
   Returns true if successful, false if a segfault occurred. */
static bool
put_user (uint8_t *udst, uint8_t byte)
{
  int error_code;
  asm ("movl $1f, %0; movb %b2, %1; 1:"
       : "=&a" (error_code), "=m" (*udst) : "q" (byte));
  return error_code != -1;
}


// Reads a 4 byte value at virtual address ADD
// Terminates the thread with exit status 11 if there is a segfault
// (including trying to read above PHYS_BASE. That's not yo memory!)
static
int get_four_bytes_user(const void * add)
{
    if(add > PHYS_BASE)
    {
        exit(11);
    }
  uint8_t *uaddr = (uint8_t *) add;
  int result = 0;
  int temp;
  temp = get_user(uaddr);

  if(temp == -1) { exit(11); }
  result += (temp << 0);

  temp = get_user(uaddr + 1);
  if(temp == -1) { exit(11); }
  result += (temp << 8);

  temp = get_user(uaddr + 2);
  if(temp == -1) { exit(11); }
  result += (temp << 16);

  temp = get_user(uaddr + 3);
  if(temp == -1) { exit(11); }
  result += (temp << 24);

  return result;
}


void
halt (void)
{
    shutdown_power_off ();
}

void
exit (int status)
{
    printf ("%s: exit(%d)\n", thread_current()->name, status);


    thread_exit ();
}

pid_t exec (const char* cmd_line)
{
    tid_t child_tid;

    child_tid = process_execute (cmd_line);

    return child_tid;
}

int
wait (pid_t pid)
{


}

bool
create (const char *file, unsigned initial_size)
{

}

bool
remove (const char *file)
{

}

int
open (const char *file)
{

}

int
filesize (int fd)
{

}

int read (int fd, void *buffer, unsigned size)
{

}

int write (int fd, const void *buffer, unsigned size)
{

}

void
seek (int fd, unsigned position)
{

}

unsigned
tell (int fd)
{

}

void
close (int fd)
{

}