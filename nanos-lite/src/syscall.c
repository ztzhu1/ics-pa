#include <common.h>
#include "syscall.h"

void yield();
void sys_yield(Context *c);
void sys_exit(Context *c);
void sys_write(Context *c);
void sys_brk(Context *c);

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0]) {
    case SYS_yield: sys_yield(c); break;
    case SYS_exit: sys_exit(c); break;
    case SYS_write: sys_write(c); break;
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

void sys_write(Context *c) {
  int fd = c->GPR2;
  const void *buf = (void *)c->GPR3;
  size_t count = c->GPR4;
  int i = 0;
  if (fd == 1 || fd == 2) {
    for (; i < count; i++) {
      putch(*(char *)(buf + i));
    }
  }
  if (i == count)
    c->GPRx = count;
  else
    c->GPRx = -1;
}

void sys_brk(Context *c) {
    // prog_break = (uintptr_t)(c->GPR2);
    c->GPRx = 0;
}