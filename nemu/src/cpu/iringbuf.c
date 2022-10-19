#include<stdio.h>
#include<string.h>
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

#define get_log(i) (iringbuf[i % IBUF_SIZE])

void print_ibuf_log() {
  for(int i = (iringbuf_index + 1) % IBUF_SIZE; i != iringbuf_index; i = (i + 1) % IBUF_SIZE) {
    printf("%s\n", iringbuf[i]);
  }
}