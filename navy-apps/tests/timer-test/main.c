#include <stdint.h>
#include <stdio.h>
#include <NDL.h>

extern int _syscall_(int, uintptr_t, uintptr_t, uintptr_t);

int main() {

  int count = 0;

  uint32_t last_time, current_time;
  last_time = NDL_GetTicks();

  while(1) {
    current_time = NDL_GetTicks();

    if(current_time - last_time > 5000) {
      last_time = current_time;
      //printf("last: %u  %u\n", last_time, current_time);

      count++;
      printf("time output %d.\n", count);
      if(count == 40) {
        break;
      }
    }
  }
  
   return 0;
}
