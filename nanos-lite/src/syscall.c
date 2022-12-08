#include <common.h>
#include "syscall.h"
#include <fs.h>
#include<timer.h>

void add_strace_log(uintptr_t *ar, uintptr_t r);
void print_sbuf_log();

uintptr_t sys_exit(int t) {
  #if defined CONFIG_STRACE && CONFIG_STRACE
    print_sbuf_log();
  #endif

  halt(t);
  return 0;
}

uintptr_t sys_yield() {
  yield();
  return 0;
}

uintptr_t sys_open(const char *pathname, int flags, int mode) {
  return fs_open(pathname, flags, mode);
}

uintptr_t sys_read(int fd, void *buf, size_t count) {
  return fs_read(fd, buf, count);
}

uintptr_t sys_write(int fd, void *buf, size_t count) {
  return fs_write(fd, buf, count);
}

uintptr_t sys_close(int fd) {
  return fs_close(fd);
}

uintptr_t sys_lseek(int fd, size_t offset, int whence) {
  return fs_lseek(fd, offset, whence);
}

uintptr_t sys_brk(void *new_brk) {
  return 0;
}

uintptr_t sys_gettimeofday(timeval *tv, timezone *tz) {
  uint64_t temp = io_read(AM_TIMER_UPTIME).us;
  tv->tv_sec = temp / 1000000;
  tv->tv_usec = temp % 1000000;
  //printf("%x  %x\n", tv->tv_sec, tv->tv_usec);
  return 0;
}


void do_syscall(Context *c) {
  uintptr_t a[4];
  
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  //printf("syscall arg is: %d  %d  %d  %d\n", a[0], a[1], a[2], a[3]);


  switch (a[0]) {
    case SYS_exit:
      #if defined CONFIG_STRACE && CONFIG_STRACE
        add_strace_log(a, c->GPRx);
      #endif
      sys_exit(a[1]);
      break;

    case SYS_yield:
      c->GPRx = sys_yield();
      #if defined CONFIG_STRACE && CONFIG_STRACE
        add_strace_log(a, c->GPRx);
      #endif
      break;

    case SYS_open:
      c->GPRx = sys_open((const char *)a[1], a[2], a[3]);
      #if defined CONFIG_STRACE && CONFIG_STRACE
        add_strace_log(a, c->GPRx);
      #endif
      break;

    case SYS_read:
      c->GPRx = sys_read(a[1], (void *)a[2], a[3]);
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

    case SYS_close:
      c->GPRx = sys_close(a[1]);
      #if defined CONFIG_STRACE && CONFIG_STRACE
        add_strace_log(a, c->GPRx);
      #endif
      break;
    
    case SYS_lseek:
      c->GPRx = sys_lseek(a[1], a[2], a[3]);
      #if defined CONFIG_STRACE && CONFIG_STRACE
        add_strace_log(a, c->GPRx);
      #endif
      break;

    
    case SYS_brk:
      c->GPRx = sys_brk((void *)a[1]);
      #if defined CONFIG_STRACE && CONFIG_STRACE
        add_strace_log(a, c->GPRx);
      #endif
      break;
    
    case SYS_gettimeofday:
      c->GPRx = sys_gettimeofday((timeval *)a[1], (timezone *)a[2]);
      #if defined CONFIG_STRACE && CONFIG_STRACE
        add_strace_log(a, c->GPRx);
      #endif
      break;

    default: panic("Unhandled syscall ID = %d", a[0]);
  }

}
