#include <cpu/decode.h>
#include "../local-include/rtl.h"

#define INSTR_LIST(f) f(add) f(sub) f(xor) f(and) f(or) f(division) f(mul) f(mulh) f(rem) f(addi) f(andi) f(xori) f(sll) f(srl) f(sra) f(slli) f(srli) f(srai) f(slt) f(sltu) f(sltiu) f(lui) f(auipc) f(beq) f(bne) f(bge) f(blt) f(bltu) f(jal) f(jalr) f(lw) f(lh) f(lhu) f(lbu) f(sw) f(sh) f(sb) f(inv) f(nemu_trap)

def_all_EXEC_ID();
