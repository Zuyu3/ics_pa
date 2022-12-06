#include <common.h>
#include "syscall.h"


#define STRACE_SIZE 100

const char call_name[][20] = {"exit", "yield"};
typedef struct 
{
    bool not_empty_flag;
    uintptr_t a[4];
    uintptr_t res;
} strace_log;

static strace_log strace_buf[STRACE_SIZE];
static int strace_index = 0;

void add_strace_log(uintptr_t *ar, uintptr_t r) {
    #ifndef CONFIG_STRACE
      return;
    #endif

    printf("Add buf at index: %d\n", strace_index);
    strace_buf[strace_index].not_empty_flag = true;
    strace_buf[strace_index].a[0] = ar[0];
    strace_buf[strace_index].a[1] = ar[1];
    strace_buf[strace_index].a[2] = ar[2];
    strace_buf[strace_index].a[3] = ar[3];
    strace_buf[strace_index].res = r;
    strace_index = (strace_index + 1) % STRACE_SIZE;
}

void print_sbuf_log() {
    #ifndef CONFIG_STRACE
      return;
    #endif


  printf("Here are the %d most recent SYSTEM CALLs.\n\n", STRACE_SIZE);
  int id = strace_index;
  do {
    if(strace_buf[id].a[0] != 0)
      printf("SYSCALL %s (%d, %d, %d, %d) : return %d\n", call_name[strace_buf[id].a[0]], strace_buf[id].a[0], strace_buf[id].a[1], strace_buf[id].a[2], strace_buf[id].a[3], strace_buf[id].res);

    id = (id + 1) % STRACE_SIZE;
  } while(id != strace_index);

  printf("\n\n");
}