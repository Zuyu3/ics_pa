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
#include <memory/paddr.h>
#include <memory/vaddr.h>

paddr_t isa_mmu_translate(vaddr_t vaddr, int len, int type) {
  paddr_t vpn1 = vaddr >> 22, vpn0 = (vaddr >> 12) & 0x3ff, offset = vaddr & 0xfff;
  paddr_t page_table1_addr = (csr.satp << 12) + vpn1 * 4;
  
  if((paddr_read(page_table1_addr, sizeof(paddr_t)) & 1) == 0) {
    printf("pc at %08x: vaddr: %08x, unmap in page table 1: %x\n", get_pc(), vaddr, page_table1_addr);
    isa_reg_display();
  }
  assert(paddr_read(page_table1_addr, sizeof(paddr_t)) & 1);
  paddr_t page_table0_addr = (paddr_read(page_table1_addr, sizeof(paddr_t)) >> 10 << 12) + vpn0 * 4;
  
  if((paddr_read(page_table0_addr, sizeof(paddr_t)) & 1) == 0) {
    printf("pc at %08x: vaddr: %08x, unmap in page table 0: %x\n", get_pc(), vaddr, page_table0_addr);
    isa_reg_display();
  }
  assert(paddr_read(page_table0_addr, sizeof(paddr_t)) & 1);
  paddr_t paddr = (paddr_read(page_table0_addr, sizeof(paddr_t)) >> 10 << 12) + offset;

  //assert(vaddr == paddr);
  return paddr;
}
