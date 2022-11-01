#include <isa.h>
#include <memory/paddr.h>
#include <memory/vaddr.h>

extern riscv32_CSR_state csr_state; 

#define VA_VPN1(va) (((uintptr_t)va) >> 22)
#define VA_VPN0(va) ((((uintptr_t)va) << 10) >> 22)
#define VA_OFFSET(va) ((uintptr_t)va & 0xfff)
#define PTE_V(pte) ((uintptr_t)pte & 0x01)
#define PTE_PPN(pte) ((uintptr_t)pte >> 10)
#define PAGE_SIZE 4096
#define PTE_SIZE 4
#define PTE_PPN_MASK (0xFFFFFC00u)

paddr_t isa_mmu_translate(vaddr_t vaddr, int len, int type) {
  // in page table 1, find PTE1, which indicates
  // page table 2's PPN
  uint32_t *pte1_ptr = (uint32_t *)(csr_state.satp.ppn * PAGE_SIZE + VA_VPN1(vaddr) * PTE_SIZE);

  assert(PTE_V(*pte1_ptr) != 0);

  // in page table 2, find PTE2(leaf PTE), which
  // helps to locate the final page of physical address
  uint32_t *pte2_ptr = (uint32_t *)(PTE_PPN(*pte1_ptr) * PAGE_SIZE + VA_VPN0(vaddr) * PTE_SIZE);

  assert(PTE_V(*pte2_ptr) != 0);

  // finally get the pa using leaf PTE
  paddr_t paddr = (paddr_t)(PTE_PPN(*pte2_ptr) * PAGE_SIZE + VA_OFFSET(vaddr));

  assert(vaddr == paddr);
  return paddr;
}
