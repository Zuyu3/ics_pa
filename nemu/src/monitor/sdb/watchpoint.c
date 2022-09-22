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

#include "sdb.h"
#include<stdio.h>

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

WP* new_wp(){
  //TODO:implement it later
  WP* p = head;
  WP* h;
  if(!free_)
    return NULL;
  if(!head){
    head = free_;
    free_ = free_->next;
    head->next = NULL;
    return head;
  }
  else if(head->NO > free_->NO) {
    p = free_->next;
    free_->next = head;
    head = free_;
    free_ = p;
    return head;
  }
  else {
    p = head;
    while(p->next && p->next->NO < free_->NO)
      p = p->next;
    h = free_;
    free_ = free_->next;
    h->next = p->next;
    p->next = h;
    return h;
  }
}

void free_wp(WP *wp) {
  WP* p = head;
  if(!head)
  return;
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
/* TODO: Implement the functionality of watchpoint */
/*
int main(){
  init_wp_pool();
  char cmd;
  int label;
  WP *p = NULL;
  while(1){
    if(scanf("%c", &cmd))
    return 0;
    switch (cmd)
    {
      case 'n':
        p = new_wp();
        if(p)
          printf("add watchpoint %d\n", p->NO);
        else
          printf("No more watchpoints left. Abandon.\n");
        break;
      case 'f':
        if(scanf("%d", &label)){
          return 0;
        }
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
        printf("%d -> ", p->NO);
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