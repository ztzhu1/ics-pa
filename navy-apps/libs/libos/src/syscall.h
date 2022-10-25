#ifndef __SYSCALL_H__
#define __SYSCALL_H__

enum {
  SYS_exit,
  SYS_yield,
  SYS_open,
  SYS_read,
  SYS_write,
  SYS_kill,
  SYS_getpid,
  SYS_close,
  SYS_lseek,
  SYS_brk,
  SYS_fstat,
  SYS_time,
  SYS_signal,
  SYS_execve,
  SYS_fork,
  SYS_link,
  SYS_unlink,
  SYS_wait,
  SYS_times,
  SYS_gettimeofday
};

extern int fs_open(const char *pathname, int flags, int mode);
extern int fs_read(int fd, void *buf, size_t count);
extern int fs_write(int fd, const void *buf, size_t count);
extern size_t fs_lseek(int fd, int offset, int whence);
extern int fs_close(int fd);

#endif
