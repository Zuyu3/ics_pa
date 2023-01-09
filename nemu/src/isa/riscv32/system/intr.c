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

#include <isa.h>
#include <generated/autoconf.h>
#include <stdio.h>

void add_ebuf_log(word_t NO, vaddr_t epc);

word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   */
  printf("\nintr raise %x\n", NO);
  #ifdef CONFIG_ETRACE
    add_ebuf_log(NO, epc);
  #endif
  csr.mcause = NO;
  csr.mepc = epc;
  csr.mstatus = (csr.mstatus & ~0x80) | ((csr.mstatus << 4) & 0x80);
  csr.mstatus &= ~0x8;
  return csr.mtvec;
}

word_t isa_query_intr() {
  if((csr.mstatus & 0x8) && cpu.INTR) {
    cpu.INTR = false;
    return IRQ_TIMER;
  }
  return INTR_EMPTY;
}
