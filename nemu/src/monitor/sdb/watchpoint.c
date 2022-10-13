#include "sdb.h"

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */

} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
WP* new_wp() {
  if (!free_) {
    printf("There is no free watchpoint!");
    exit(-1);
  }
  WP *p_free = free_;
  if (!free_->next) {
    free_ = NULL;
  }
  else {
    while(1) {
      if (!p_free->next->next) {
        WP *p = p_free;
        p_free = p_free->next;
        p->next = NULL;
        break;
      }
    }
  }

  WP *p_head = head;
  if (!p_head)
    p_head = p_free;
  else {
    while(p_head->next)
      p_head = p_head->next;
    p_head->next = p_free;
  }
  return p_free;
}

void free_wp(WP *wp) {
  WP *p_free = free_;
  while (p_free->next)
    p_free = p_free->next;

  WP *p_head = head;
  if (wp == p_head) {
    wp->next = NULL;
    p_free->next = wp;
    head = p_head->next;
  }
  else {
    while (p_head->next != wp && !p_head->next)
      p_head = p_head->next;
    if (!p_head->next) {
      printf("wp is already free!\n");
      exit(-1);
    }
    p_head->next = wp->next;
    wp->next = NULL;
    p_free->next = wp;
  }
}

WP *getWP(int index) {
  return wp_pool + index;
}