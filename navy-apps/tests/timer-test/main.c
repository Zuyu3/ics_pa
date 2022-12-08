#include <stdint.h>
#include <stdio.h>


typedef struct
{
    uint64_t tv_sec;     /* seconds */
    uint64_t tv_usec;    /* microseconds */
} timeval;

int gettimeofday(struct timeval *tv, struct timeval *tz);


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
  printf("%ld  %ld\n", last_time.tv_sec, last_time.tv_usec);

  while(1) {
    gettimeofday(&current_time, NULL);
    printf("%ld  %ld\n", last_time.tv_sec, last_time.tv_usec);
    printf("%ld  %ld\n\n\n", current_time.tv_sec, current_time.tv_usec);

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
