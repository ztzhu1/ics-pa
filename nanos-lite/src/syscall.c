#include <common.h>
#include "syscall.h"

void yield();
void sys_yield(Context *c);
void sys_exit(Context *c);
void sys_open(Context *c);
void sys_read(Context *c);
void sys_write(Context *c);
void sys_lseek(Context *c);
void sys_close(Context *c);
void sys_brk(Context *c);

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0]) {
    case SYS_yield: sys_yield(c); break;
    case SYS_exit: sys_exit(c); break;
    case SYS_open: sys_open(c); break;
    case SYS_read: sys_read(c); break;
    case SYS_write: sys_write(c); break;
    case SYS_lseek: sys_lseek(c); break;
    case SYS_close: sys_close(c); break;
    case SYS_brk: sys_brk(c); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}

void sys_yield(Context *c) {
  yield();
  c->GPRx = 0;
}

void sys_exit(Context *c) {
  halt(0);
}

void sys_brk(Context *c) {
  // always success
  c->GPRx = 0;
}

void sys_open(Context *c) {
  const char *pathname = (char *)c->GPR2;
  int flags = c->GPR3;
  int mode = c->GPR4;
  int fd = fs_open(pathname, flags, mode);
  c->GPRx = fd;
}

void sys_read(Context *c) {
  int fd = c->GPR2;
  void *buf = (void *)c->GPR3;
  size_t count = c->GPR4;
  c->GPRx = fs_read(fd, buf, count);
}

void sys_write(Context *c) {
  int fd = c->GPR2;
  const void *buf = (void *)c->GPR3;
  size_t count = c->GPR4;
  c->GPRx = fs_write(fd, buf, count);
}

void sys_lseek(Context *c) {
  int fd = c->GPR2;
  int offset = c->GPR3;
  int whence = c->GPR4;
  c->GPRx = fs_lseek(fd, offset, whence);
}

void sys_close(Context *c) {
  int fd = c->GPR2;
  c->GPRx = fs_close(fd);
}