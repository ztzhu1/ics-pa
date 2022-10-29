#include <proc.h>

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

void naive_uload(PCB *pcb, const char *filename);
Context *context_kload(PCB *pcb, void (*entry)(void *), void *arg);
Context *context_uload(PCB *pcb, char *path, char *const argv[], char *const envp[]);

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  volatile int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite with arg '%p' for the %dth time!", (uintptr_t)arg, j);
    j ++;
    yield();
  }
}

void init_proc() {
  char *argv[] = {"argv1", "argv2"};
  context_kload(&pcb[0], hello_fun, (void *)0);
  context_uload(&pcb[1], "/bin/timer-test", argv, NULL);
  switch_boot_pcb();

  Log("Initializing processes...");

  // load program here
  // naive_uload(NULL, "/bin/hello");

}

Context* schedule(Context *prev) {
  current->cp = prev;
  current = current == &pcb[0] ? &pcb[1] : &pcb[0];
  return current->cp;
}
