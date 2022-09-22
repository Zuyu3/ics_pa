/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include"sdb.h"
#include"watchpoint.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
    memset(wp_pool[i].expr, 0, sizeof(wp_pool[i].expr));
    wp_pool[i].last_value = 0;
  }

  head = NULL;
  free_ = wp_pool;
}

WP* new_wp(char *e){
  //TODO:implement it later
  WP* p = head;
  WP* h;
  if(!free_)
    return NULL;
  if(!head){
    head = free_;
    free_ = free_->next;
    head->next = NULL;
    h = head;
  }
  else if(head->NO > free_->NO) {
    p = free_->next;
    free_->next = head;
    head = free_;
    free_ = p;
    h = head;

  }
  else {
    p = head;
    while(p->next && p->next->NO < free_->NO)
      p = p->next;
    h = free_;
    free_ = free_->next;
    h->next = p->next;
    p->next = h;
  }
  memset(h->expr, 0, sizeof(h->expr));
  strcpy(h->expr, e);
  return h;
}

void free_wp(int x) {
  if(x >= NR_WP || x < 0) {
    printf("Error. watchpoint index overflow.\n");
    return;
  }
  WP *wp = wp_pool + x;
  WP* p = head;
  if(!head)
    return;
  if(head == wp) {
    wp->next = free_;
    free_ = wp;
    head = NULL;
    return;
  }
  while(p->next){
    if(p->next == wp){
        p->next = p->next->next;
        break;
    }
    p = p->next;
  }
  if(!free_) {
    free_ = wp;
    wp->next = NULL;
    return;
  }
  else if(free_->NO > wp->NO) {
    p->next = wp->next;
    wp->next = free_;
    free_ = wp;
    return;
  }
  else {
    p = free_;
    while(p->next && p->next->NO < wp->NO)
      p = p->next;
    wp->next = p->next;
    p->next = wp;
    return;
  }
}

void print_watchpoints() {
  WP* p = head;
  if(!p) {
    printf("No watchpoint.\n");
    return;
  }
  printf("%-10s%-20s%-25s%s\n", "Num", "Type", "Disp Enb Address", "What");
  while(p != NULL) {
    printf("%-10d%-20s%-25s%s\n", p->NO, "watchpoint", "keep y", p->expr);
    p = p->next;
  }
  return;
}

/* TODO: Implement the functionality of watchpoint */
/*
int main(){
  init_wp_pool();
  char cmd;
  int label;
  char *toEval[20000];
  WP *p = NULL;
  while(1){
    scanf("%c", &cmd);
    switch (cmd)
    {
      case 'n':
        scanf("%s", toEval);
        p = new_wp(toEval);
        if(p)
          printf("add watchpoint %d\n", p->NO);
        else
          printf("No more watchpoints left. Abandon.\n");
        break;
      case 'f':
        scanf("%d", &label);
        free_wp(wp_pool + label);
        printf("free watchpoint %d\n", wp_pool[label].NO);
        break;
      case 'q':
        return 0;
      default:
        break;
      }
      printf("working watchpoints are:\n");
      p = head;
      while(p){
        printf("%d  %s\n", p->NO, p->expr);
        p = p->next;
      }
      printf("\nfree watchpoints are:\n");
      p = free_;
      while(p){
        printf("%d -> ", p->NO);
        p = p->next;
      }
      printf("\n\n\n");
  }
  return 0;
}
*/