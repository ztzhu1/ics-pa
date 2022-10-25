#include <proc.h>
#include <elf.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

enum {SEEK_SET, SEEK_CUR, SEEK_END};

int fs_open(const char *pathname, int flags, int mode);
int fs_read(int fd, void *buf, size_t count);
int fs_write(int fd, const void *buf, size_t count);
size_t fs_lseek(int fd, int offset, int whence);
int fs_close(int fd);

static void read(int fd, void *buf, size_t count, int offset) {
 fs_lseek(fd, offset, SEEK_SET);
 fs_read(fd, buf, count);
}

static void load_segment(int fd, int offset, uintptr_t virtAddr, size_t fileSize, size_t memSize) {
  read(fd, (void *)virtAddr, fileSize, offset);
  if (fileSize < memSize) { // .bss
    memset((void *)(virtAddr + fileSize), 0, memSize - fileSize);
  }
}

static uintptr_t loader(PCB *pcb, const char *filename) {
  Elf_Ehdr elf_header;
  Elf_Phdr prog_header;
  int fd = fs_open(filename, 0, 0);
  read(fd, &elf_header, sizeof(elf_header), 0);

  assert(*(uint32_t *)(elf_header.e_ident) == 0x464c457f);

  size_t n = sizeof(prog_header);
  for (int i = 0; i < elf_header.e_phnum; i++) {
    read(fd, &prog_header, n, i * n + elf_header.e_phoff);
    if (prog_header.p_type == PT_LOAD) {
      printf("load segment %d\n", i);
      load_segment(fd, prog_header.p_offset, prog_header.p_vaddr, prog_header.p_filesz, prog_header.p_memsz);
    }
  }
  fs_close(fd);
  return elf_header.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %08x", entry);
  ((void(*)())entry) ();
}

