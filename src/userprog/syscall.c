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

static void syscall_halt (struct intr_frame *f);
static void syscall_exit(struct intr_frame *f);
static pid_t syscall_exec (struct intr_frame *f);
static int syscall_wait (struct intr_frame *f);
static bool syscall_create (struct intr_frame *f);
static bool syscall_remove (struct intr_frame *f);
static int syscall_open (struct intr_frame *f);
static int syscall_filesize (struct intr_frame *f);
static int syscall_read (struct intr_frame *f);
static int syscall_write (struct intr_frame *f);
static void syscall_seek (struct intr_frame *f);
static unsigned syscall_tell (struct intr_frame *f);
static void syscall_close (struct intr_frame *f);

struct fd_entry* open_file(struct thread *t, int fd);
int close_file(struct thread* t, int fd, bool all);
void ExitStatus(int status);
static inline bool is_valid_stack_pointer(const void  *vaddr);

/* Returns true if UADDR is a valid, mapped user address,
   false otherwise. */
static bool
verify_user (const void *uaddr)
{
  return (uaddr < PHYS_BASE &&
          uaddr > CODESEG_BASE &&
          pagedir_get_page (thread_current ()->pagedir, uaddr) != NULL);
}

/* Reads a byte at user virtual address UADDR.
   UADDR must be below PHYS_BASE.
   Returns the byte value if successful, -1 if a segfault
   occurred. */
static int
get_user (const uint8_t *uaddr)
{
  if(!is_user_vaddr(uaddr) || uaddr < CODESEG_BASE)
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
  if (!verify_user(f->esp)) {
    ExitStatus(-1);
  }

  int num = *((int *)(f->esp));

  if (num >= MAX_SYSCALL || num < 0)
    {
      printf("We don't have this System Call!\n");
      ExitStatus(-1);
    }
  if (!syscall_handlers[num])
    {
      printf("this System Call %d not Implement!\n", num);
      ExitStatus(-1);
    }
  if (syscall_handlers[num](f) == -1) {
    ExitStatus(-1);
  }
}

static void
syscall_halt (struct intr_frame *f) {
  shutdown_power_off();
  f->eax = 0;
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

//   // empty string or null
//   if (!file_name || strlen(file_name) == 0) {
//     f->eax = -1;
//     return -1;
//   } else {
//     struct file* _file = filesys_open(file_name);
//     // missing file
//     if (_file == NULL) {
//       f->eax = -1;
//       return -1;
//     }
//     struct file_entry *fn = malloc (sizeof(struct file_entry));
//     // memory alloc failed
//     if (fn == NULL) {
//       f->eax = -1;
//       return -1;
//     }
//     struct thread* cur = thread_current();

//     fn->f = _file;
//     fn->fd = cur->next_fd;
//     cur->next_fd += 1;
//     cur->opened_file_num += 1;
//     list_push_back(&cur->file_table, &fn->elem);
//     f->eax = fn->fd;
//     return fn->fd;
//   }
}

static int
syscall_filesize (struct intr_frame *f) {

}

static int
syscall_read (struct intr_frame *f) {

}

static int
syscall_write (struct intr_frame *f) {
  int *esp = (int *)f->esp;
  if (!is_user_vaddr(esp + 7)) {
    ExitStatus(-1);
  }

  int fd = *(esp + 2);
  unsigned size = *(esp + 3);
  char *buffer = (char *)*(esp + 6);

  if (fd == STDOUT_FILENO) {
    putbuf(buffer, size);
    f->eax = 0;
  } else {
    struct thread* cur = thread_current();
    struct fd_entry *fn = open_file(cur, fd);
    if (!fn) {
      f->eax = 0;
      return;
    }
    f->eax = file_write(fn->file, buffer, size);
  }
}

static void
syscall_seek (struct intr_frame *f) {

}

static unsigned
syscall_tell (struct intr_frame *f) {

}

static void
syscall_close (struct intr_frame *f) {
  if (!is_user_vaddr(((int *)f->esp) + 2)) {
    ExitStatus(-1);
  }
  struct thread* cur = thread_current();
  int fd = *((int *)f->esp + 1);
  f->eax = close_file(cur, fd, false);
}

int
close_file (struct thread* t, int fd, bool all) {
  struct list_elem *e;
  if (all) {
    while (!list_empty(&t->file_table)) {
      struct fd_entry *fn = list_entry(list_pop_front(&t->file_table), struct fd_entry, elem);
      file_close(fn->file);
      free(fn);
    }
    t->opened_file_num = 0;
    return 0;
  } else {
    struct fd_entry *fn;
    for (e = list_begin(&t->file_table); e != list_end(&t->file_table); e = list_next(e)) {
      fn = list_entry(e, struct fd_entry, elem);
      if (fn->fd == fd) {
        list_remove(e);
        if (fd == t->next_fd - 1) {
          t->next_fd -= 1;
        }
        t->opened_file_num -= 1;
        file_close(fn->file);
        free(fn);
        return 0;
      }
    }
  }
}

struct fd_entry*
open_file(struct thread *t, int fd) {
  struct list_elem *e;
  for (e = list_begin(&t->file_table); e != list_end(&t->file_table); e = list_next(e)) {
    struct fd_entry* fn = list_entry(e, struct fd_entry, elem);
    if (fn->fd == fd) {
      return fn;
    }
  }
  return NULL;
}

// non-normal exit.
void
ExitStatus(int status)
{
  struct thread *cur = thread_current();
  cur->ret=status;
  thread_exit();
}


static inline bool
is_valid_stack_pointer(const void *vaddr)
{
  return (vaddr <= PHYS_BASE && vaddr > CODESEG_BASE);
}
