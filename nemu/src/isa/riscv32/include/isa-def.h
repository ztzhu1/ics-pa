#ifndef __ISA_RISCV32_H__
#define __ISA_RISCV32_H__

#include <common.h>

typedef struct {
  struct {
    rtlreg_t _32;
  } gpr[32];

  vaddr_t pc;
} riscv32_CPU_state;

typedef struct {
    rtlreg_t mepc; // 0x341
    rtlreg_t mtvec; // 0x305
    rtlreg_t mcause; // 0x342
    rtlreg_t mstatus; // 0x300
    union {
      struct {
        uint32_t ppn21_0  : 22;
        uint32_t asid30_22:  9;
        uint32_t mode31_31:  1;
      };
      rtlreg_t value;
    } satp;
} riscv32_CSR_state;

// decode
typedef struct {
  union {
    struct {
      uint32_t opcode1_0 : 2;
      uint32_t opcode6_2 : 5;
      uint32_t rd        : 5;
      uint32_t funct3    : 3;
      uint32_t rs1       : 5;
      uint32_t rs2       : 5;
      uint32_t funct7    : 7;
    } r;
    struct {
      uint32_t opcode1_0 : 2;
      uint32_t opcode6_2 : 5;
      uint32_t rd        : 5;
      uint32_t funct3    : 3;
      uint32_t rs1       : 5;
      int32_t  simm11_0  :12;
    } i;
    struct {
      uint32_t opcode1_0 : 2;
      uint32_t opcode6_2 : 5;
      uint32_t imm4_0    : 5;
      uint32_t funct3    : 3;
      uint32_t rs1       : 5;
      uint32_t rs2       : 5;
      int32_t  simm11_5  : 7;
    } s;
    struct {
      uint32_t opcode1_0 : 2;
      uint32_t opcode6_2 : 5;
      uint32_t imm11_11  : 1;
      uint32_t imm4_1    : 4;
      uint32_t funct3    : 3;
      uint32_t rs1       : 5;
      uint32_t rs2       : 5;
      uint32_t imm10_5   : 6;
      int32_t  simm12_12 : 1;
    } b;
    struct {
      uint32_t opcode1_0 : 2;
      uint32_t opcode6_2 : 5;
      uint32_t rd        : 5;
      uint32_t imm31_12  :20;
    } u;
    struct {
      uint32_t opcode1_0 : 2;
      uint32_t opcode6_2 : 5;
      uint32_t rd        : 5;
      uint32_t imm19_12  : 8;
      uint32_t imm11_11  : 1;
      uint32_t imm10_1    :10;
      int32_t simm20_20  : 1;
    } j;
    uint32_t val;
  } instr;
} riscv32_ISADecodeInfo;

extern riscv32_CSR_state csr_state;

#define isa_mmu_check(vaddr, len, type) (csr_state.satp.mode31_31 ? MMU_TRANSLATE : MMU_DIRECT)

#endif
