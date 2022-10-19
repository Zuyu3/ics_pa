#include<stdio.h>
#include<string.h>
#include <generated/autoconf.h>
#define IBUF_SIZE 50
#define IBUF_LENGTH 100

static char iringbuf[IBUF_SIZE][IBUF_LENGTH];
static int iringbuf_index = 0;

void add_ibuf_log(char *ilog) {
    #ifndef CONFIG_ITRACE_COND
      return;
    #endif
    strcpy(iringbuf[iringbuf_index], ilog);
    iringbuf_index = (iringbuf_index + 1) % IBUF_SIZE;
}

void print_ibuf_log() {
  #ifndef CONFIG_ITRACE_COND
    return;
  #endif
  printf("Here are the %d most recent instructions executed before the program error\n\n", IBUF_SIZE);
  for(int i = (iringbuf_index + 1) % IBUF_SIZE; i != iringbuf_index; i = (i + 1) % IBUF_SIZE) {
    if(!strcmp(iringbuf[i], ""))
      continue;
    printf("      %s\n", iringbuf[i]);
  }
  printf("--->  %s\n", iringbuf[iringbuf_index]);
}