#include "userprog/syscall.h"
#include "userprog/pagedir.h"
#include "userprog/process.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "threads/malloc.h"
#include "kernel/console.h"
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "devices/shutdown.h"
#include "lib/string.h"

#define MAX_SYSCALL 21
#define CODESEG_BASE ((void *) 0x08048000)
typedef int pid_t;

static void syscall_handler (struct intr_frame *);
/* Array of syscall functions */
static int (*syscall_handlers[MAX_SYSCALL]) (struct intr_frame *);

void ExitStatus(int status);

/* Reads a byte at user virtual address UADDR.
   UADDR must be below PHYS_BASE.
   Returns the byte value if successful, -1 if a segfault
   occurred. */
static int
get_user (const uint8_t *uaddr)
{
  if(!is_user_vaddr(uaddr))
    return -1;
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
  if(!is_user_vaddr(udst))
    return false;
  int error_code;
  asm ("movl $1f, %0; movb %b2, %1; 1:"
       : "=&a" (error_code), "=m" (*udst) : "q" (byte));
  return error_code != -1;
}


static bool is_valid_pointer(void * esp, uint8_t argc){
  uint8_t i = 0;
  for (; i < argc; ++i)
  {
    if (get_user(((uint8_t *)esp)+i) == -1){
      return false;
    }
  }
  return true;
}

static bool is_valid_string(void * str)
{
  int ch=-1;
  while((ch=get_user((uint8_t*)str++))!='\0' && ch!=-1);
    if(ch=='\0')
      return true;
    else
      return false;
}


// non-normal exit.
static void
kill_program(void) {
  struct thread *cur = thread_current();
  cur->ret = -1;
  thread_exit();
}



static void
syscall_halt (struct intr_frame *f) {
  shutdown_power_off();
  f->eax = 0;
}

static void
syscall_exit (struct intr_frame *f) {
  if (!is_user_vaddr(((int *)f->esp) + 2)) {
    kill_program();
  }

  struct thread *cur = thread_current();
  cur->ret = *((int *)f->esp + 1);
  f->eax = 0;
  thread_exit();
}

static pid_t
syscall_exec (struct intr_frame *f) {
  
}
static int
syscall_wait (struct intr_frame *f) {

}
static bool
syscall_create (struct intr_frame *f) {

}

static bool
syscall_remove (struct intr_frame *f) {

}

static int
syscall_open (struct intr_frame *f) {
  if (!is_valid_pointer(f->esp + 4, 4) ||
      !is_valid_string(*(char **)(f->esp + 4))) {
    f->eax = -1;
    return -1;
  }
  const char* file_name = (char *)*((int *)f->esp + 1);
  f->eax = process_open(file_name);
  return 0;
}

static int
syscall_filesize (struct intr_frame *f) {
  if (!is_valid_pointer(f->esp + 4, 4)) {
    return -1;
  }
  int fd = * (int * )(f->esp + 4);
  f->eax = process_filesize(fd);
  return 0;
}

static int
syscall_read (struct intr_frame *f) {
  if (!is_valid_pointer(f->esp + 4, 12)) {
    return -1;
  }
  int fd = *(int *)(f->esp + 4);
  void *buffer = *(char **)(f->esp + 8);
  unsigned size = *(unsigned *)(f->esp + 12);

  if (!is_valid_pointer(buffer, 1) || !is_valid_pointer(buffer + size, 1)) {
    return -1;
  }
  int written_size = process_read(fd, buffer, size);
  f->eax = written_size;
  return 0;
}

static int
syscall_write (struct intr_frame *f) {
  if (!is_valid_pointer(f->esp + 4, 12)) {
      return -1;
  }
  int fd = *(int *)(f->esp + 4);
  void *buffer = *(char **)(f->esp + 8);
  unsigned size = *(unsigned *)(f->esp + 12);

  if (!is_valid_pointer(buffer, 1) || !is_valid_pointer(buffer + size, 1)) {
    return -1;
  }
  int written_size = process_write(fd, buffer, size);
  f->eax = written_size;
  return 0;
}

static void
syscall_seek (struct intr_frame *f) {
  if (!is_valid_pointer(f->esp + 4, 4)) {
    return -1;
  }
  int fd = *(int *)(f->esp + 4);
  unsigned pos = *(unsigned *)(f->esp + 8);
  f->eax = process_tell(fd, pos);
  return 0;
}

static unsigned
syscall_tell (struct intr_frame *f) {
  if (!is_valid_pointer(f->esp + 4, 4)) {
      return -1;
  }
  int fd = *(int *)(f->esp + 4);
  f->eax = process_tell(fd);
  return 0;
}

static void
syscall_close (struct intr_frame *f) {
  if (!is_valid_pointer(f->esp + 4, 4)) {
    return -1;
  }
  int fd = *(int *)(f->esp + 4);
  process_close(fd);
  return 0;
}

void
syscall_init (void)
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
  for (int i = 0; i < MAX_SYSCALL; i++) {
    syscall_handlers[i] = NULL;
  }

  syscall_handlers[SYS_HALT] = &syscall_halt;
  syscall_handlers[SYS_EXIT] = &syscall_exit;
  syscall_handlers[SYS_EXEC] = &syscall_exec;
  syscall_handlers[SYS_WAIT] = &syscall_wait;
  syscall_handlers[SYS_CREATE] = &syscall_create;
  syscall_handlers[SYS_REMOVE] = &syscall_remove;
  syscall_handlers[SYS_OPEN] = &syscall_open;
  syscall_handlers[SYS_FILESIZE] = &syscall_filesize;
  syscall_handlers[SYS_READ] = &syscall_read;
  syscall_handlers[SYS_WRITE] = &syscall_write;
  syscall_handlers[SYS_SEEK] = &syscall_seek;
  syscall_handlers[SYS_TELL] = &syscall_tell;
  syscall_handlers[SYS_CLOSE] = &syscall_close;
};

static void
syscall_handler (struct intr_frame *f)
{
  if (!is_valid_pointer(f->esp, 4)) {
    kill_program();
  }

  int num = *((int *)(f->esp));

  if (num >= MAX_SYSCALL || num < 0)
    {
      printf("We don't have this System Call!\n");
      kill_program();
    }
  if (!syscall_handlers[num])
    {
      printf("this System Call %d not Implement!\n", num);
      kill_program();
    }
  if (syscall_handlers[num](f) == -1) {
    kill_program();
  }
}
