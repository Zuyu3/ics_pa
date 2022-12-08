#include <stdint.h>
#include <stdio.h>

typedef struct
{
    uint64_t tv_sec;     /* seconds */
    uint64_t tv_usec;    /* microseconds */
} timeval;

int gettimeofday(timeval *tv, timeval *tz);

timeval last_time;
timeval current_time;

extern int _syscall_(int, uintptr_t, uintptr_t, uintptr_t);

int main() {

  int count = 0;

  printf("%p  %p\n", &last_time, &current_time);
  
  gettimeofday(&last_time, NULL);
  printf("%lld  %lld\n", last_time.tv_sec, last_time.tv_usec);

  while(1) {
    gettimeofday(&current_time, NULL);
    printf("current: %lld  last: %lld\n", current_time.tv_sec, last_time.tv_sec);

    if((current_time.tv_sec - last_time.tv_sec) > 0) {
      last_time.tv_usec = current_time.tv_usec;
      last_time.tv_sec = current_time.tv_sec;
      count++;
      printf("time output %d.\n", count);
      if(count == 40) {
        break;
      }
    }
  }
  
   return 0;
}
