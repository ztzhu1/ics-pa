#include "sdb.h"

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  char *expression;
  word_t val;

} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
    wp_pool[i].expression = NULL;
  }

  head = NULL;
  free_ = wp_pool;
}

bool isTail(WP *p) {
  return p->next == NULL;
}

/* TODO: Implement the functionality of watchpoint */
WP* new_wp(char *expression) {
  if (!free_) {
    printf("There is no free watchpoint!");
    exit(-1);
  }
  WP *p_free = free_;
  free_ = free_->next;
  p_free->next = NULL;

  if (!head)
    head = p_free;
  else {
    WP *p_head = head;
    while(!isTail(p_head))
      p_head = p_head->next;
    p_head->next = p_free;
  }
  p_free->expression = (char *) malloc(strlen(expression) + 1);
  strcpy(p_free->expression, expression);
  bool success;
  p_free->val = expr(expression, &success);
  return p_free;
}

void free_wp(WP *wp) {
  WP *p_free = free_;
  while (!isTail(p_free))
    p_free = p_free->next;

  if (wp == head) {
    p_free->next = wp;
    head = head->next;
  }
  else {
    WP *p_head = head;
    while (p_head && p_head->next != wp)
      p_head = p_head->next;
    if (!p_head) {
      printf("wp%d is already free!\n", wp->NO);
      exit(-1);
    }
    p_head->next = wp->next;
    p_free->next = wp;
  }
  wp->next = NULL;
  if (wp->expression) free(wp->expression);
  wp->expression = NULL;
}

WP *getWP(int index) {
  return wp_pool + index;
}

void wp_display() {
  printf("using:\n");
  bool success;
  for (int i = 0; i < NR_WP; i++) {
    if (wp_pool[i].expression)
      printf("wp%-2d: %s, %d\n", i, wp_pool[i].expression, expr(wp_pool[i].expression, &success));
  }  

  printf("free:\n");
  int count = 0;
  for (int i = 0; i < NR_WP; i++) {
    if (!wp_pool[i].expression) {
      printf("wp%-2d ", i);
      count++;
    }
    if (count % 4 == 0 && count != 0)
      printf("\n");
  }  
  if (count % 4 != 0)
    printf("\n");
}

WP *anyChange(bool *changed, word_t *new_val) {
  for (int i = 0; i < NR_WP; i++) {
    if (wp_pool[i].expression) {
      bool success;
      word_t val = expr(wp_pool[i].expression, &success);
      if (val != wp_pool[i].val) {
        *changed = true;
        *new_val = val;
        return wp_pool + i;
      }
    }
  }
  *changed = false;
  return NULL;
}