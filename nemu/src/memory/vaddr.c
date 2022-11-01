#include <isa.h>
#include <isa-def.h>
#include <memory/paddr.h>

paddr_t isa_mmu_translate(vaddr_t vaddr, int len, int type);

word_t vaddr_ifetch(vaddr_t addr, int len) {
  if (isa_mmu_check(addr, 0, 0) == MMU_TRANSLATE)
    addr = isa_mmu_translate(addr, 0, 0);
  return paddr_read(addr, len);
}

word_t vaddr_read(vaddr_t addr, int len) {
  if (isa_mmu_check(addr, 0, 0) == MMU_TRANSLATE)
    addr = isa_mmu_translate(addr, 0, 0);
  return paddr_read(addr, len);
}

void vaddr_write(vaddr_t addr, int len, word_t data) {
  if (isa_mmu_check(addr, 0, 0) == MMU_TRANSLATE)
    addr = isa_mmu_translate(addr, 0, 0);
  paddr_write(addr, len, data);
}
