#include <am.h>
#include <nemu.h>
#include<stdio.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  kbd->keycode = inl(KBD_ADDR);
  printf("%d %d\n", inl(KBD_ADDR), inl(KBD_ADDR + 4));

  kbd->keydown = kbd->keycode? true: false;
  if(kbd->keydown) {
  }
}
