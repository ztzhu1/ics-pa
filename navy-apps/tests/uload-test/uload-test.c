#include <stdint.h>
#include <stdio.h>

#ifdef __ISA_NATIVE__
#error can not support ISA=native
#endif

#define SYS_yield 1
extern int _syscall_(int, uintptr_t, uintptr_t, uintptr_t);

int main(int argc, char *argv[], char *envp[]) {
  int j = 1;
  while (1) {
    printf("Hello World from dummy with arg '%s' and envp '%s' for the %dth time!\n", argv[0], envp[0], j);
    j++;
    _syscall_(SYS_yield, 0, 0, 0);
  }
  return 0;
}
