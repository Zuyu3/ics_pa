#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>

struct timeval last_time, current_time;

extern int _syscall_(int, uintptr_t, uintptr_t, uintptr_t);

int main() {

  int count = 0;

  gettimeofday(&last_time, NULL);

  while(1) {
    gettimeofday(&current_time, NULL);
    printf("current: %ld  %ld\n", current_time.tv_sec, current_time.tv_usec);

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
  
   return 0;
}
