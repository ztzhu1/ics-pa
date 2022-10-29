#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {
  int argc = *(int *)args;
  char **argv = (char **)((uintptr_t)args + 4);
  char **envp = (char **)((uintptr_t)args + 4 * (argc + 2));
  printf("%s\n", argv[0]);
  printf("%s\n", argv[1]);
  printf("%s\n", envp[0]);
  printf("%s\n", envp[1]);
  environ = envp;
  exit(main(argc, argv, envp));
  assert(0);
}
