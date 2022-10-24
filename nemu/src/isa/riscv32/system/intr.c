#include <isa.h>

extern riscv32_CSR_state csr_state;

word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   */
  csr_state.mcause = NO;
  csr_state.mepc = epc;

  return (word_t)csr_state.mtvec;
}

word_t isa_query_intr() {
  return INTR_EMPTY;
}

rtlreg_t *map_csr_addr(riscv32_CSR_state *csr_state, paddr_t csr_paddr) {
  rtlreg_t *csr_vaddr;
  switch (csr_paddr)
  {
    case 0x300:
      csr_vaddr = &csr_state->mstatus;
      break;
    case 0x305:
      csr_vaddr = &csr_state->mtvec;
      break;
    case 0x341:
      csr_vaddr = &csr_state->mepc;
      break;
    case 0x342:
      csr_vaddr = &csr_state->mcause;
      break;
    default:
      assert(0);
  }
  return csr_vaddr;
}