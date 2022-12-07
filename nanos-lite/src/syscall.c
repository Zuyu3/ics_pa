#include <common.h>
#include "syscall.h"

void add_strace_log(uintptr_t *ar, uintptr_t r);
void print_sbuf_log();

uintptr_t sys_yield() {
  yield();
  return 0;
}

uintptr_t sys_exit(int t) {
  #if defined CONFIG_STRACE && CONFIG_STRACE
    print_sbuf_log();
  #endif

  halt(t);
  return 0;
}

uintptr_t sys_write(int fd, const void * buf, size_t count) {
  //TODO: implement (fd==stderr) and error return later.
  if(fd < 1 || fd > 2) {
    panic("ERROR in system write: fd == %d\n", fd);
    return -1;
  }
  for(int i = 0; i < count; i++) {
    putch(((char *)buf)[i]);
  }
  return count;
}


void do_syscall(Context *c) {
  uintptr_t a[4];
  
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  printf("syscall arg is: %d  %d  %d  %d\n", a[0], a[1], a[2], a[3]);


  switch (a[0]) {
    case SYS_exit:
      #if defined CONFIG_STRACE && CONFIG_STRACE
        add_strace_log(a, c->GPRx);
      #endif
      sys_exit(0);
      break;

    case SYS_yield:
      c->GPRx = sys_yield();
      #if defined CONFIG_STRACE && CONFIG_STRACE
        add_strace_log(a, c->GPRx);
      #endif
      break;


    case SYS_write:
      c->GPRx = sys_write(a[1], (void *)a[2], a[3]);
      #if defined CONFIG_STRACE && CONFIG_STRACE
        add_strace_log(a, c->GPRx);
      #endif
      break;


    default: panic("Unhandled syscall ID = %d", a[0]);
  }

}
