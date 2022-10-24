#include <isa.h>
#include <cpu/difftest.h>
#include "../local-include/reg.h"
#include <common.h>


bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  bool ok = true;
  for (int i = 0; i < 32; i++) {
      bool ok_temp = difftest_check_reg(reg_name(i, 4), pc, ref_r->gpr[i]._32, cpu.gpr[i]._32);
      if (ok == true)
        ok = ok_temp;
  }
  if (ref_r->pc != pc) {
    ok = false;
    Log("PC is different, right = %08x, wrong = %08x", ref_r->pc, pc);
  }
  return ok;
}

void isa_difftest_attach() {
}
