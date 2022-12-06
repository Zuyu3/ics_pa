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
  printf("syscall arg is: %d  %d  %d  %d\n", a[0], a[1], a[2], a[3]);


  switch (a[0]) {
    case SYS_yield:
      c->GPRx = sys_yield();
      #ifdef CONFIG_STRACE
        add_strace_log(a, c->GPRx);
      #endif
      break;
    
    case SYS_exit:
      #ifdef CONFIG_STRACE
        add_strace_log(a, c->GPRx);
      #endif
      sys_exit(0);
      break;
    
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

}
