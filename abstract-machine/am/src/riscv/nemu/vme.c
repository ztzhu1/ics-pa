#include <am.h>
#include <nemu.h>
#include <klib.h>

static AddrSpace kas = {};
static void* (*pgalloc_usr)(int) = NULL;
static void (*pgfree_usr)(void*) = NULL;
static int vme_enable = 0;

static Area segments[] = {      // Kernel memory mappings
  NEMU_PADDR_SPACE
};

#define USER_SPACE RANGE(0x40000000, 0x80000000)

#define VPN0(x) (((uint32_t)x << 10) >> 22) // VA[21:12]
#define VPN1(x) ((uint32_t)x >> 22) // VA[31:22]
#define OFFSET(x) ((uint32_t)x & 0x00000FFF) // VA[11:0]

#define PPN(x) ((uint32_t)x >> 10) // PTE[31:10]
#define PTE_PPN_MASK (0xFFFFFC00u) // PTE[31:10]
#define PTE_V 0x01 // PTE[0:0] Valid
#define PTE_R 0x02
#define PTE_W 0x04
#define PTE_X 0x08
#define PTE_U 0x10
#define PTE_A 0x40 // PTE[6:6] Accessed
#define PTE_D 0x80 // PTE[7:7] Dirty

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

void map(AddrSpace *as, void *va, void *pa, int prot) {
  pa = (void *)(((uintptr_t)pa) & (~0xfff));

  PTE *page_table_entry = as->ptr + VPN1(va) * 4;

  if (!(*page_table_entry & PTE_V)){ // not valid
    void *alloced_page = pgalloc_usr(PGSIZE);
    *page_table_entry = (*page_table_entry & ~PTE_PPN_MASK) | (((uintptr_t)alloced_page >> 2) & PTE_PPN_MASK);
    *page_table_entry = (*page_table_entry | PTE_V);
  }

  PTE *leaf_page_table_entry = (PTE *)(PPN(*page_table_entry) * 4096 + VPN0(va) * 4);
  *leaf_page_table_entry = (PTE_PPN_MASK & ((uintptr_t)pa >> 2)) | (PTE_V | PTE_R | PTE_W | PTE_X) | (prot ? PTE_U : 0);
}

Context *ucontext(AddrSpace *as, Area kstack, void *entry) {
  Context *c = kstack.end - sizeof(Context);
  c->mepc = (uintptr_t)entry;
  c->pdir = as->ptr;
  return c;
}
