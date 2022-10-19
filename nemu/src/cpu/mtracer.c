#include<stdio.h>
#include<string.h>
#include <generated/autoconf.h>
#include<common.h>

#define MBUF_SIZE 50
#define MBUF_LENGTH 100

static char mringbuf[MBUF_SIZE][MBUF_LENGTH];
static int mringbuf_index = 0;

void add_mbuf_log(int read_or_write, paddr_t addr, int len, word_t data) {
    #ifndef CONFIG_MTRACE
      return;
    #endif

    sprintf(mringbuf[mringbuf_index], "%c :    %-10x  %-3d   %-10d    0x%x", read_or_write? 'r': 'w', addr, len, data, data);

    mringbuf_index = (mringbuf_index + 1) % MBUF_SIZE;
}


void print_mbuf_log() {
  #ifndef CONFIG_MTRACE
    return;
  #endif
  printf("Here are the %d most recent memory operates before the program error\n", MBUF_SIZE);
  printf("operation     addr     len    data       hex data\n\n");
  for(int i = (mringbuf_index + 1) % MBUF_SIZE; i != mringbuf_index; i = (i + 1) % MBUF_SIZE) {
    if(!strcmp(mringbuf[i], ""))
      continue;
    printf("%s\n", mringbuf[i]);
  }
}
