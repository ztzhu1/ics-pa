#include <stdlib.h>
#include <isa.h>
#include "local-include/reg.h"

#define RESET   "\033[0m"
#define RED     "\033[31m"

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

void isa_reg_display() {
  int col = 8;
  int row = 4;
  bool *success = (bool *) malloc(1);
   for (int i = 0; i < row; i++) {
    for (int j = 0; j < col; j++) {
      int idx = i * col + j;
      const char *reg = regs[idx];
      word_t val = isa_reg_str2val(reg, success);
      if (!*success) {
        printf(RED "Wrong register index!" RESET);
        exit(-1);
      }
      uint8_t *val_ptr = (uint8_t *)&val;
      printf("%-3s: ", reg);
      for (int m = 0; m < 4; m++)
        printf("%02x ", *(val_ptr + m));
      printf("\n");
    }
  }
  free(success);
  success = NULL;
}

word_t isa_reg_str2val(const char *s, bool *success) {
  if (strcmp(s, "pc") == 0) {
    // *success = true;
    return cpu.pc;
  }

  word_t val = 0;
  int idx;
  for (idx = 0; idx < 32; idx++) {
    if (strcmp(s, regs[idx]) == 0)
      break;
  }
  if (idx < 32) {
    val = gpr(idx);
    if (success != NULL)
      *success = true;
  } else {
    if (success != NULL)
      *success = false;
  }
  return val;
}
