#include <stdio.h>
#include <NDL.h>

int main() {
  NDL_Init(0);
  while (1) {
    char buf[64];
    if (NDL_PollEvent(buf, sizeof(buf))) {
      printf("\n###########\n");
      printf("receive event: %s\n", buf);
      printf("************\n\n");
    }
  }
  return 0;
}
