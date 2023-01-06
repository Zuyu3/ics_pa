#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;

void call_main(uintptr_t *args) {
  int argc = *(int *)args;
  char **argv, **envp;
  argv = *((void **)args + 1);
  envp = *((void **)args + 2);
  environ = envp;
  exit(main(argc, argv, envp));
  assert(0);
}
