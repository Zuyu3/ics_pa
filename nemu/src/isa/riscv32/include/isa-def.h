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

#ifndef __ISA_RISCV32_H__
#define __ISA_RISCV32_H__

#include <common.h>

typedef struct {
  word_t gpr[32];
  vaddr_t pc;
  bool INTR;
} riscv32_CPU_state;

//TODO:Add it when doing PA3. Not sure.
typedef union {
  struct 
  {
    word_t satp;      //0x180

    union
    {
      word_t mstatus;
      char m[sizeof(word_t)];
    } mstatus;        //0x300
    
    vaddr_t mtvec;    //0x305

    vaddr_t mepc;     //0x341
    word_t mcause;    //0x342
  };
  word_t as_array[5];

} riscv32_CSR_state;

// decode
typedef struct {
  union {
    uint32_t val;
  } inst;
} riscv32_ISADecodeInfo;

extern riscv32_CSR_state csr;
#define isa_mmu_check(vaddr, len, type) (csr.satp >> 30 ? MMU_TRANSLATE : MMU_DIRECT)

#endif
