#include <proc.h>

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite with arg '%p' for the %dth time!", (uintptr_t)arg, j);
    j ++;
    yield();
  }
}

void naive_uload(PCB *pcb, const char *filename);
void context_kload(PCB *pcb, void(*entry)(void *), void *arg);
void context_uload(PCB *pcb, char *filename, char *const argv[], char *const envp[]);

void init_proc() {
  // context_kload(&pcb[0], hello_fun, NULL);
  char *const argv0[] = {"a from argv0", NULL};
  char *const envp0[] = {"envp0=foo", NULL};
  char *const argv1[] = {"a from argv1", NULL};
  char *const envp1[] = {"envp1=foo", NULL};
  context_uload(&pcb[0], "/bin/uload-test", argv0, envp0);
  context_uload(&pcb[1], "/bin/uload-test", argv1, envp1);
  switch_boot_pcb();

  Log("Initializing processes...");

  // load program here
  // naive_uload(NULL, "/bin/timer-test");

}

Context* schedule(Context *prev) {
  current->cp = prev;
  current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);
  return current->cp;
}
