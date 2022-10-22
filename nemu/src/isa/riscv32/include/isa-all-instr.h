#include <cpu/decode.h>
#include "../local-include/rtl.h"

#define INSTR_LIST(f) f(add) f(sub) f(xor) f(and) f(or) f(division) f(mul) f(divu) f(mulh) f(mulhu) f(rem) f(remu) f(addi) f(andi) f(xori) f(ori) f(sll) f(srl) f(sra) f(slli) f(slti) f(srli) f(srai) f(slt) f(sltu) f(sltiu) f(csrrw) f(csrrs) f(ecall) f(mret) f(lui) f(auipc) f(beq) f(bne) f(bge) f(blt) f(bltu) f(bgeu) f(jal) f(jalr) f(lw) f(lh) f(lb) f(lhu) f(lbu) f(sw) f(sh) f(sb) f(inv) f(nemu_trap)

def_all_EXEC_ID();
