#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {
  int argc = *(int *)args;
  char *argv[] = *((char **)args + 1);
  char *envp[] = *((char **)args + 1);
  printf("%d  %p  %p\n", argc, argv, envp);
  environ = envp;
  exit(main(argc, argv, envp));
  assert(0);
}
