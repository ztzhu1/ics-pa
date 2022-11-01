#include <isa.h>
#include <memory/paddr.h>
#include <memory/vaddr.h>

extern riscv32_CSR_state csr_state; 

#define VA_VPN1(va) (((vaddr_t)va) >> 22)
#define VA_VPN0(va) (((vaddr_t)va << 10) >> 22)
#define VA_OFFSET(va) ((vaddr_t)va & 0xfff)
#define PTE_PPN(pte) ((uint32_t)pte >> 10)
#define PTE_SIZE 4
#define PTE_PPN_MASK (0xFFFFFC00u)
#define PTE_V (0x01)

paddr_t isa_mmu_translate(vaddr_t vaddr, int len, int type) {
  // in page table 1, find PTE1, which indicates
  // page table 2's PPN
  uint32_t pte1_ptr = (uint32_t)(csr_state.satp.ppn * PAGE_SIZE + VA_VPN1(vaddr) * PTE_SIZE);
  word_t pte1 = paddr_read(pte1_ptr, 4);

  assert((pte1 & PTE_V) != 0);

  // in page table 2, find PTE2(leaf PTE), which
  // helps to locate the final page of physical address
  uint32_t pte2_ptr = (uint32_t)(PTE_PPN(pte1) * PAGE_SIZE + VA_VPN0(vaddr) * PTE_SIZE);
  word_t pte2 = paddr_read(pte2_ptr, 4);
  assert((pte2 & PTE_V) != 0);

  // finally get the pa using leaf PTE
  paddr_t paddr = (paddr_t)(PTE_PPN(pte2) * PAGE_SIZE + VA_OFFSET(vaddr));

  assert(vaddr == paddr);
  return paddr;
}
