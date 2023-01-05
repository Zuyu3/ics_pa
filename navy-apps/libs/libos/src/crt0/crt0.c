#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {
  printf("call main get argu: %p\n", args);
  int argc = *(int *)args;
  char **argv, **envp;
  argv = *((void **)args + 1);
  envp = *((void **)args + 2);
  printf("call main get: %d  %p  %p\n", argc, argv, envp);
  assert(0);
  environ = envp;
  exit(main(argc, argv, envp));
  assert(0);
}
