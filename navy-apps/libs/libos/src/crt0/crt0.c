#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {
  uint8_t *sp = (uint8_t *)args;
  int argc = *sp++;
  char *argv[10];
  char *envp[10];

  int i;
  for (i = 0; *sp != NULL; i++) {
    if (*sp == '\0') {
      if (*(sp + 1) == NULL) break;
      else sp++;
    }
    argv[i] = sp;
    while (*sp != '\0') sp++;
  }
  for (i = 0; *sp != NULL; i++) {
    if (*sp == '\0') {
      if (*(sp + 1) == NULL) break;
      else sp++;
    }
    envp[i] = sp;
    while (*sp != '\0') sp++;
  }

  environ = envp;
  exit(main(argc, argv, envp));
  assert(0);
}
