#include <am.h>
#include <nemu.h>
#include <klib.h>

riscv32_CSR_state csr_state;

static AddrSpace kas = {};
static void* (*pgalloc_usr)(int) = NULL;
static void (*pgfree_usr)(void*) = NULL;
static int vme_enable = 0;

static Area segments[] = {      // Kernel memory mappings
  NEMU_PADDR_SPACE
};

#define USER_SPACE RANGE(0x40000000, 0x80000000)

static inline void set_satp(void *pdir) {
  uintptr_t mode = 1ul << (__riscv_xlen - 1);
  asm volatile("csrw satp, %0" : : "r"(mode | ((uintptr_t)pdir >> 12)));
}

static inline uintptr_t get_satp() {
  uintptr_t satp;
  asm volatile("csrr %0, satp" : "=r"(satp));
  return satp << 12;
}

bool vme_init(void* (*pgalloc_f)(int), void (*pgfree_f)(void*)) {
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  kas.ptr = pgalloc_f(PGSIZE);

  int i;
  for (i = 0; i < LENGTH(segments); i ++) {
    void *va = segments[i].start;
    for (; va < segments[i].end; va += PGSIZE) {
      map(&kas, va, va, 0);
    }
  }

  set_satp(kas.ptr);
  vme_enable = 1;

  return true;
}

void protect(AddrSpace *as) {
  PTE *updir = (PTE*)(pgalloc_usr(PGSIZE));
  as->ptr = updir;
  as->area = USER_SPACE;
  as->pgsize = PGSIZE;
  // map kernel space
  memcpy(updir, kas.ptr, PGSIZE);
}

void unprotect(AddrSpace *as) {
}

void __am_get_cur_as(Context *c) {
  c->pdir = (vme_enable ? (void *)get_satp() : NULL);
}

void __am_switch(Context *c) {
  if (vme_enable && c->pdir != NULL) {
    set_satp(c->pdir);
  }
}

#define VA_VPN1(va) (((uintptr_t)va) >> 22)
#define VA_VPN0(va) ((((uintptr_t)va) << 10) >> 22)
#define VA_OFFSET(va) ((uintptr_t)va & 0xfff)
#define PTE_V(pte) ((uintptr_t)pte & 0x01)
#define PTE_PPN(pte) ((uintptr_t)pte >> 10)
#define PAGE_SIZE 4096
#define PTE_SIZE 4
#define PTE_PPN_MASK (0xFFFFFC00u)

void map(AddrSpace *as, void *va, void *pa, int prot) {
  // in page table 1, find PTE1, which indicates
  // page table 2's PPN
  uint32_t *pte1_ptr = (uint32_t *)((uintptr_t)as->ptr + PTE_SIZE * VA_VPN1(va));
  if (PTE_V(*pte1_ptr) == 0) { 
    /* can be freely used by any softwares
     * i.e. not alloced */
    // alloc page for table 2
    void *page_start = pgalloc_usr(PGSIZE);
    // calculate ppn
    uint32_t ppn = ;
    // reserve status bit and set V = 1
    *pte1_ptr = (*pte1_ptr & ~PTE_PPN_MASK) | 0x01;
    // write ppn into pte1
    *pte1_ptr = (ppn << 10) | *pte1_ptr;
  }
  // in page table 2, find PTE2(leaf PTE), which
  // helps to locate the physical address
  uint32_t *pte2_ptr = (uint32_t *)(PTE_PPN(*pte1_ptr) * PAGE_SIZE + VA_VPN0(va) * PTE_SIZE);
  // finally get the pa using leaf PTE
  // void *pa = (void *)(PTE_PPN(*pte2_ptr) * PAGE_SIZE + VA_OFFSET(va));
  *pte2_ptr = ((uint32_t)(((uintptr_t)pa - VA_OFFSET(va)) / PAGE_SIZE) << 10) | 0x01;
}

Context *ucontext(AddrSpace *as, Area kstack, void *entry) {
  Context *c = kstack.end - sizeof(Context);
  // TODO: Now is machine mode. Should have been user mode.
  c->mstatus = 0x1800;
  c->mepc = (uintptr_t)entry;
  return c;
}
