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

static void load_segment(size_t offset, uintptr_t virtAddr, size_t fileSize, size_t memSize) {
  uint8_t buf[24000];
  ramdisk_read(buf, offset, fileSize);
  memcpy((void *)virtAddr, buf, fileSize);
  if (fileSize < memSize) {
    memset((void *)(virtAddr + fileSize), 0, memSize - fileSize);
  }
    return;
}

static uintptr_t loader(PCB *pcb, const char *filename) {
  Elf_Ehdr elf_header;
  Elf_Phdr prog_header;
  ramdisk_read(&elf_header, 0x0, sizeof(elf_header));
  assert(*(uint32_t *)(elf_header.e_ident) == 0x464c457f);
  for (int i = 0; i < elf_header.e_phnum; i++) {
    size_t n = sizeof(prog_header);
    ramdisk_read(&prog_header, i * n + elf_header.e_phoff, n);
    if (prog_header.p_type == PT_LOAD) {
      printf("load segment %d\n", i);
      load_segment(prog_header.p_offset, prog_header.p_vaddr, prog_header.p_filesz, prog_header.p_memsz);
    }
  }
  return elf_header.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %08x", entry);
  ((void(*)())entry) ();
}

