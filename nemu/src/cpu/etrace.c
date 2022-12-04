#include<stdio.h>
#include<string.h>
#include <generated/autoconf.h>
#include<common.h>

#define ETRACE_SIZE 100
typedef struct 
{
    vaddr_t mepc;
    word_t mcause;
} etrace_log;
static etrace_log etrace_buf[ETRACE_SIZE];
static int etrace_index = 0;

void add_ebuf_log(word_t NO, vaddr_t epc) {
    #ifndef CONFIG_ETRACE
      return;
    #endif

    etrace_buf[etrace_index].mcause = NO;
    etrace_buf[etrace_index].mepc = epc;
    etrace_index = (etrace_index + 1) % ETRACE_SIZE;
}

void print_ebuf_log() {
  #ifndef CONFIG_ETRACE
    return;
  #endif
  printf("Here are the %d most recent Excepts.\n\n", ETRACE_SIZE);
  for(int i = (etrace_index + 1) % ETRACE_SIZE; i != etrace_index; i = (i + 1) % ETRACE_SIZE) {
    printf("At mepc = 0x%08x,  mcause: 0x%08x", etrace_buf[i].mepc, etrace_buf[i].mcause);
  }
}
