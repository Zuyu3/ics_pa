#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

static Context* (*user_handler)(Event, Context*) = NULL;

void __am_get_cur_as(Context *c);
void __am_switch(Context *c);

Context* __am_irq_handle(Context *c) {
  __am_get_cur_as(c);
  if (user_handler) {
    Event ev = {0};
    switch (c->mcause) {
      case 8: 
        //printf("c-> GPR1 is: %d\n", c->GPR1);
        if(c->GPR1 == -1) {
          ev.event = EVENT_YIELD;
        }
        else {
          ev.event = EVENT_SYSCALL;
        }
        c->mepc += 4;
        break;
      case 0x80000007:
        ev.event = EVENT_IRQ_TIMER;
        break;
      default: ev.event = EVENT_ERROR; break;
    }

    c = user_handler(ev, c);
    assert(c != NULL);
  }
  __am_switch(c);
  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  //mtvec = 0x80000528 (from asm)
  //if DIFFTEST is enabled, mtvec = 0x80000538
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));

  #ifdef CONFIG_DIFFTEST
  uint32_t temp = 0x1800u;
  asm volatile("csrw mstatus, %0" 
               : :"r"(temp));
  #endif

  // register event handler
  user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  Context *ctp = kstack.end - sizeof(Context);
  ctp->mepc = (uintptr_t)entry;
  ctp->mstatus = 0x1880;
  ctp->gpr[0] = 0;
  ctp->gpr[10] = (uintptr_t)arg;
  ctp->pdir = NULL;
  //TODO: Maybe some problem here.
  ctp->gpr[2] = (uintptr_t)kstack.start + 4;  //set stack pointer sp
  return ctp;
}

void yield() {
  asm volatile("li a7, -1; ecall");
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
