#include <common.h>
#include "syscall.h"

uintptr_t sys_yield() {
  yield();
  return 0;
}

uintptr_t sys_exit() {
  halt(0);
  return 0;
}


void do_syscall(Context *c) {
  uintptr_t a[4];
  
  a[0] = c->GPR1;
  printf("syscall arg 0 is: %d\n", a[0]);
  /*
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c-> GPR4
  */

  switch (a[0]) {
    case SYS_yield:
      c->GPRx = sys_yield();
      break;
    case SYS_exit:
      sys_exit();
      break;
      
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
