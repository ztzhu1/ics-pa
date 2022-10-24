#include <proc.h>
#include <elf.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);

extern uint8_t ramdisk_start;
extern uint8_t ramdisk_end;

static void load_segment(size_t offset, size_t virtAddr, size_t fileSize, size_t memSize) {
  char buf[24000];
  ramdisk_read(buf, offset, fileSize);
  memcpy((void *)virtAddr, buf, fileSize);
  if (fileSize < memSize) {
    memset((void *)(virtAddr + fileSize), 0, memSize - fileSize);
  }
    return;
}

static uintptr_t loader(PCB *pcb, const char *filename) {
  Elf_Ehdr elfe;
  Elf_Phdr elfp;
  ramdisk_read(&elfe, 0x0, sizeof(elfe));
  assert(*(uint32_t *)(elfe.e_ident) == 0x464c457f);
  for (int i = 0; i < elfe.e_phnum; i++) {
    size_t n = sizeof(elfp);
    ramdisk_read(&elfp, i * n + elfe.e_phoff, n);
    if (elfp.p_type == PT_LOAD) {
      printf("load segment %d\n", i);
      load_segment(elfp.p_offset, elfp.p_vaddr, elfp.p_filesz, elfp.p_memsz);
    }
  }
  return elfe.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %08x", entry);
  // printf("0x830000b4: %08x\n", *(uint32_t *)0x830000b4);
  entry = 0x83000120;
  ((void(*)())entry) ();
}

