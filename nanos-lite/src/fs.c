#include <fs.h>

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t file_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_EVENT, FD_DISP, FD_FB, FD_TEXT};

extern size_t serial_write(const void *buf, size_t offset, size_t len);
extern size_t events_read(void *buf, size_t offset, size_t len);
extern size_t dispinfo_read(void *buf, size_t offset, size_t len);
extern size_t fb_write(const void *buf, size_t offset, size_t len);

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

static size_t file_read(void *buf, size_t offset, size_t len) {
  size_t count = ramdisk_read(buf, offset, len);
  return count;
}

static size_t file_write(const void *buf, size_t offset, size_t len) {
  size_t count = ramdisk_write(buf, offset, len);
  return count;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, serial_write},
  [FD_EVENT] = {"/dev/events", 0, 0, events_read, invalid_write},
  [FD_DISP] = {"/proc/dispinfo", 0, 0, dispinfo_read, invalid_write},
  [FD_FB] = {"/dev/fb", 0, 0, invalid_read, fb_write},
#include "files.h"
};

static inline size_t num_files() {
  return sizeof(file_table) / sizeof(Finfo);
}

size_t file_size(int fd) {
  return file_table[fd].size;
}

size_t fs_lseek(int fd, int offset, int whence);

void init_fs() {
  // TODO: initialize the size of /dev/fb
  AM_GPU_CONFIG_T config = io_read(AM_GPU_CONFIG);
  file_table[FD_FB].size = config.width * config.height * sizeof(uint32_t);

  for (size_t i = FD_TEXT; i < num_files(); i++) {
    file_table[i].write = file_write;
    file_table[i].read = file_read;
  }
}

int fs_open(const char *pathname, int flags, int mode) {
  int n = num_files();
  int i = 0;
  int fd = -1;
  for (; i < n; i++) {
    if (strcmp(pathname, file_table[i].name) == 0) {
      fd = i;
      file_table[fd].file_offset = 0;
      break;
    }
  }
  return fd;
}

int fs_read(int fd, void *buf, size_t count) {
  assert((fd == 0 || fd > 2) && fd < num_files());
  size_t offset = file_table[fd].disk_offset + file_table[fd].file_offset;
  if (fd >= FD_FB) {
    int capacity = file_table[fd].size - file_table[fd].file_offset;
    if (count > capacity)
      count = capacity;
  }
  count = file_table[fd].read(buf, offset, count);
  if (count != -1)
      file_table[fd].file_offset += count;
  return count;
}

int fs_write(int fd, const void *buf, size_t count) {
  assert(fd != 0 && fd < num_files());
  size_t offset = file_table[fd].disk_offset + file_table[fd].file_offset;
  if (fd >= FD_FB) {
    int capacity = file_table[fd].size - file_table[fd].file_offset;
    if (count > capacity)
      count = capacity;
  }
  count = file_table[fd].write(buf, offset, count);
  if (count != -1)
      file_table[fd].file_offset += count;
  return count;
}

size_t fs_lseek(int fd, int offset, int whence) {
  int start = 0; 
  int cur = file_table[fd].file_offset;
  int end = file_table[fd].size;
  size_t new_offset;
  if (whence == SEEK_SET)
    new_offset = start + offset;
  else if (whence == SEEK_CUR)
    new_offset = cur + offset;
  else if (whence == SEEK_END)
    new_offset = end + offset;
  else
    assert(0);
  assert(new_offset >= start && new_offset <= end);
  file_table[fd].file_offset = new_offset;
  return new_offset;
}

int fs_close(int fd) {
  file_table[fd].file_offset = 0;
  return 0;
}