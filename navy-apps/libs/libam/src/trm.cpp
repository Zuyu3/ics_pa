#include <am.h>
#include <unistd.h>
Area heap;


void putch(char ch) {
    if(!write(1, &ch, 1))
      return;
}

void halt(int code) {
    _exit(code);
}
