#include <stdint.h>
#include <stdio.h>
#include <fixedptc.h>
#include <assert.h>

float a, b;

int main() {
  a = 1.452;
  b = 2.151;
  assert(fixedpt_muli(a, 5) == fixedpt_rconst(a * 5));
   return 0;
}
