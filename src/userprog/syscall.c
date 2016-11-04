#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "kernel/console.h"
#define MAX_SYSCALL 21

static void syscall_handler (struct intr_frame *);
/* Array of syscall functions */
static int (*syscall_handlers[MAX_SYSCALL]) (struct intr_frame *);

static void syscall_halt (struct intr_frame *f);
static void syscall_exit(struct intr_frame *f);
static void syscall_exec (struct intr_frame *f);
static void syscall_wait (struct intr_frame *f);
static void syscall_create (struct intr_frame *f);
static void syscall_remove (struct intr_frame *f);
static void syscall_open (struct intr_frame *f);
static void syscall_filesize (struct intr_frame *f);
static void syscall_read (struct intr_frame *f);
static void syscall_write (struct intr_frame *f);
static void syscall_seek (struct intr_frame *f);
static void syscall_tell (struct intr_frame *f);
static void syscall_close (struct intr_frame *f);



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
  if (!is_user_vaddr(f->esp)) {
    printf("Wrong stack\n");
    ExitStatus(-1);
  }

  int num = *((int *)f->esp);
  if (num >= MAX_SYSCALL || num < 0) {
    printf("Wrong system call number!\n");
    ExitStatus(-1);
  }

  if (!syscall_handlers[num]) {
    printf("This system call is not implemented yet!\n");
    ExitStatus(-1);
  }

  syscall_handlers[num](f);
}

static void
syscall_halt (struct intr_frame *f) {

}

static void
syscall_exit (struct intr_frame *f) {
  if (!is_user_vaddr(((int *)f->esp) + 2)) {
    ExitStatus(-1);
  }

  struct thread *cur = thread_current();
  cur->ret = *((int *)f->esp + 1);
  f->eax = 0;
  thread_exit();
}

static void
syscall_exec (struct intr_frame *f) {

}
static void
syscall_wait (struct intr_frame *f) {

}
static void
syscall_create (struct intr_frame *f) {

}



static void
syscall_remove (struct intr_frame *f) {

}

static void
syscall_open (struct intr_frame *f) {

}

static void
syscall_filesize (struct intr_frame *f) {

}

static void
syscall_read (struct intr_frame *f) {

}

static void
syscall_write (struct intr_frame *f) {
  int *esp = (int *)f->esp;

  if (!is_user_vaddr(esp + 7)) {
    printf("invalid user vaddr\n");
    ExitStatus(-1);
  }

  int fd = *(esp + 2);
  unsigned size = *(esp + 3);
  char *buffer = (char *)*(esp + 6);

  if (fd == STDOUT_FILENO) {
    putbuf(buffer, size);
    f->eax = 0;
    return 0;
  }
}

static void
syscall_seek (struct intr_frame *f) {

}

static void
syscall_tell (struct intr_frame *f) {

}

static void
syscall_close (struct intr_frame *f) {

}




// non normal exit.
void
ExitStatus(int status)
{
  struct thread *cur=thread_current();
  cur->ret=status;
  thread_exit();
}
