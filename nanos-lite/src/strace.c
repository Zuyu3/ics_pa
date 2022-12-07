#include <common.h>
#include "syscall.h"
char *get_file_name(int file_id);


#define STRACE_SIZE 100

const char call_name[][20] = {"exit ", "yield", "open", "read", "write", "kill", "getpid",
                              "close", "lseek", "brk"};
typedef struct 
{
    bool not_empty_flag;
    uintptr_t a[4];
    uintptr_t res;
} strace_log;

static strace_log strace_buf[STRACE_SIZE];
static int strace_index = 0;

void print_single_log(int index) {
    uintptr_t temp[4];
    for(int i = 0; i < 4; i++) {
      temp[i] = strace_buf[index].a[i];
    }

    if(strace_buf[index].not_empty_flag) {
      switch (temp[0])
      {
      case 2:
      case 3:
      case 4:
      case 7:
      case 8:
        printf("SYSCALL (%d, %d, %d, %d)  %s  \"%s\": return %d\n", temp[0], temp[1], temp[2], temp[3], call_name[temp[0]], get_file_name(temp[1]), strace_buf[index].res);
        break;
      
      default:
        printf("SYSCALL (%d, %d, %d, %d)  %s  : return %d\n", temp[0], temp[1], temp[2], temp[3], call_name[temp[0]], strace_buf[index].res);
        break;
      }
    }
}


void add_strace_log(uintptr_t *ar, uintptr_t r) {
    #if !(defined CONFIG_STRACE) || CONFIG_STRACE == 0
      return;
    #endif

    //printf("Add strace log at index=%d\n", strace_index);

    strace_buf[strace_index].not_empty_flag = true;
    strace_buf[strace_index].a[0] = ar[0];
    strace_buf[strace_index].a[1] = ar[1];
    strace_buf[strace_index].a[2] = ar[2];
    strace_buf[strace_index].a[3] = ar[3];
    strace_buf[strace_index].res = r;


    #if CONFIG_STRACE == 2
      print_single_log(strace_index);
    #endif

    strace_index = (strace_index + 1) % STRACE_SIZE;


}

void print_sbuf_log() {
    #if !(defined CONFIG_STRACE) || CONFIG_STRACE == 0
      return;
    #endif


  printf("Here are the %d most recent SYSTEM CALLs.\n\n", STRACE_SIZE);
  int id = strace_index;
  do {
    print_single_log(id);
    id = (id + 1) % STRACE_SIZE;
  } while(id != strace_index);

  printf("\n\n");
}