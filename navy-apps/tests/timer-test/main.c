#include <stdint.h>
#include <sys/time.h>

#ifdef __ISA_NATIVE__
#error can not support ISA=native
#endif

#define SYS_yield 1
extern int _syscall_(int, uintptr_t, uintptr_t, uintptr_t);

int main() {
  struct timeval last_time;
  struct timeval current_time;
  int count = 0;
  
  gettimeofday(&last_time, NULL);

  while(1) {
    gettimeofday(&current_time, NULL);
    if((current_time.tv_usec - last_time.tv_usec) > 500000) {
      last_time.tv_usec = current_time.tv_usec;
      last_time.tv_sec = current_time.tv_sec;
      count++;
      printf("time output %d.\n", count);
      if(count == 40) {
        break;
      }
    }
  }
  
  return _syscall_(SYS_yield, 0, 0, 0);
}
