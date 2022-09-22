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
  if(!free_)
    return NULL;
  if(!head){
    head = free_;
    free_ = free_->next;
    head->next = NULL;
    return head;
  }
  while(p->next)
    p = p->next;
  p->next = free_;
  free_ = free_->next;
  p = p->next;
  p->next = NULL;
  return p;
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

