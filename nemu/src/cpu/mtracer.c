#include<stdio.h>
#include<string.h>
#include <generated/autoconf.h>

#define MBUF_SIZE 50
#define MBUF_LENGTH 100

static char mringbuf[MBUF_SIZE][MBUF_LENGTH];
static int mringbuf_index = 0;


void add_mbuf_log(char *ilog) {
    #ifndef CONFIG_MTRACE
      return;
    #endif

    strcpy(mringbuf[mringbuf_index], ilog);
    mringbuf_index = (mringbuf_index + 1) % MBUF_SIZE;
}


void print_ibuf_log() {
  #ifndef CONFIG_MTRACE
    return;
  #endif
  printf("Here are the %d most recent instructions executed before the program error\n", MBUF_SIZE);
  for(int i = (mringbuf_index + 1) % MBUF_SIZE; i != mringbuf_index; i = (i + 1) % MBUF_SIZE) {
    if(!strcmp(mringbuf[i], ""))
      continue;
    printf("%s\n", mringbuf[i]);
  }
}
