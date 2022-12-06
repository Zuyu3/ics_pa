#include <common.h>
#include "syscall.h"

void add_strace_log(uintptr_t *ar, uintptr_t r);
void print_sbuf_log();

uintptr_t sys_yield() {
  printf("do sys yield\n");
  yield();
  return 0;
}

uintptr_t sys_exit(int t) {
  #ifdef CONFIG_STRACE
    print_sbuf_log();
  #endif

  halt(t);
  return 0;
}


void do_syscall(Context *c) {
  uintptr_t a[4];
  
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  //printf("syscall arg 0 is: %d\n", a[0]);


  switch (a[0]) {
    case SYS_yield:
      c->GPRx = sys_yield();
      add_strace_log(a, c->GPRx);
      printf("%p\n", a);
      break;
    /*
    case SYS_exit:
      add_strace_log(a, c->GPRx);
      sys_exit(0);
      break;
    */
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  #ifdef CONFIG_STRACE
  #endif
}
