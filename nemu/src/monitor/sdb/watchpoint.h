#include"sdb.h"

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  char expr[5000];
  word_t last_value;
  struct watchpoint *next;


  /* TODO: Add more members if necessary */

} WP;

void init_wp_pool();
WP* new_wp(char *e);
void free_wp(int x);
void print_watchpoints();