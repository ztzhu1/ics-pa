#include <isa.h>
#include <cpu/cpu.h>
#include <cpu/decode.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  nemu_state.state = NEMU_QUIT;
  return -1;
}

static int cmd_si(char *args) {
  int n = 1;
  if (args) n = atoi(args);
  if (n > 0) cpu_exec(n);
  return 0;
}

void wp_display();
static int cmd_info(char *args) {
  if (strcmp(args, "r") == 0) isa_reg_display();
  else if (strcmp(args, "w") == 0) wp_display();
  return 0;
}

static int cmd_x(char *args) {
  char *n_str = strtok(args, " ");
  char *exp = n_str + strlen(n_str) + 1; 
  int n = atoi(n_str);
  paddr_t pc;
  pc = expr(exp, NULL);

  uint8_t* guest_to_host(paddr_t paddr); 
  for (int i = 0; i < n; i++) {
    uint8_t *instr_start = guest_to_host(pc);
    word_t instr_val = *(uint32_t *) instr_start;
    uint8_t *instr = (uint8_t *)&instr_val;
    for (int j = 0; j < 4; j++) 
      printf("%02x ", instr[j]);
    printf("\n");
    pc += 4;
  }
  return 0;
}

static int cmd_p(char *args) {
  printf("%d\n", expr(args, NULL));
  return 0;
}

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  char *expression;
} WP;

WP* new_wp(char *expression);
WP *getWP(int index);
void free_wp(WP *wp);

static int cmd_w(char *args) {
  WP *wp = new_wp(args);
  printf("wp%d is watching \"%s\"\n", wp->NO, wp->expression);
  return 0;
}

static int cmd_d(char *args) {
  WP *wp = getWP(atoi(args));
  free_wp(wp);
  printf("wp%d is free.\n", wp->NO);
  return 0;
}

static int cmd_help(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },

  /* TODO: Add more commands */
  { "si", "execute n steps", cmd_si },
  { "info", "print info", cmd_info },
  { "x", "scan memory", cmd_x },
  { "p", "calculate expression value", cmd_p },
  { "w", "create watchpoint", cmd_w },
  { "d", "delete watchpoint", cmd_d },
};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
