#include <isa.h>
#include <memory/paddr.h>
#include <memory/vaddr.h>

extern riscv32_CSR_state csr_state;

#define VPN0(x) (((vaddr_t)x << 10) >> 22) // VA[21:12]
#define VPN1(x) ((vaddr_t)x >> 22) // VA[31:22]
#define OFFSET(x) ((vaddr_t)x & 0x00000FFF) // VA[11:0]

#define PPN(x) ((paddr_t)x >> 10) // PTE[31:10]
#define PTE_V 0x01 // PTE[0:0] Valid
#define PTE_A 0x40 // PTE[6:6] Accessed
#define PTE_D 0x80 // PTE[7:7] Dirty

paddr_t isa_mmu_translate(vaddr_t vaddr, int len, int type) {
  paddr_t page_table_entry_addr = csr_state.satp.ppn21_0 * 4096 + VPN1(vaddr) * 4;
  uint32_t page_table_entry = paddr_read(page_table_entry_addr, 4);
  assert(page_table_entry & PTE_V);

  paddr_t leaf_page_table_entry_addr = PPN(page_table_entry) * 4096 + VPN0(vaddr) * 4;
  uint32_t leaf_page_table_entry = paddr_read(leaf_page_table_entry_addr, 4);
  assert(leaf_page_table_entry & PTE_V);


  if (type == 0) { // Accessed. Set PTE_A to 1
    paddr_write(leaf_page_table_entry_addr, 4, leaf_page_table_entry | PTE_A);
  } else if (type == 1) { // Writen. Set PTE_D to 1
    paddr_write(leaf_page_table_entry_addr, 4, leaf_page_table_entry | PTE_D);
  }

  paddr_t pa = PPN(leaf_page_table_entry) * 4096 + OFFSET(vaddr);
  return pa;
}
