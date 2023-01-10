#define concat_temp(x, y) x ## y
#define concat(x, y) concat_temp(x, y)
#define MAP(c, f) c(f)

#if __riscv_xlen == 32
#define LOAD  lw
#define STORE sw
#define XLEN  4
#else
#define LOAD  ld
#define STORE sd
#define XLEN  8
#endif

#define REGS(f) \
      f( 1)       f( 3) f( 4) f( 5) f( 6) f( 7) f( 8) f( 9) \
f(10) f(11) f(12) f(13) f(14) f(15) f(16) f(17) f(18) f(19) \
f(20) f(21) f(22) f(23) f(24) f(25) f(26) f(27) f(28) f(29) \
f(30) f(31)

#define PUSH(n) STORE concat(x, n), (n * XLEN)(sp);
#define POP(n)  LOAD  concat(x, n), (n * XLEN)(sp);

#define CONTEXT_SIZE  ((32 + 3 + 2) * XLEN)
#define OFFSET_SP     ( 2 * XLEN)
#define OFFSET_CAUSE  (32 * XLEN)
#define OFFSET_STATUS (33 * XLEN)
#define OFFSET_EPC    (34 * XLEN)
#define OFFSET_NP     (36 * XLEN)

.align 3
.globl __am_asm_trap
__am_asm_trap:

  csrrw sp, mscratch, sp   // (1) atomically exchange sp and mscratch
  bnez sp, save_context    // (2) take the branch if we trapped from user
  csrr sp, mscratch        // (3) if we trapped from kernel, restore the original sp
  
  //now sp=ksp, mscratch=$sp, only need to save mscratch in c->gpr[sp]

save_context:
  addi sp, sp, -CONTEXT_SIZE
  MAP(REGS, PUSH)
  //all registers are free now

  csrr t0, mcause
  csrr t1, mstatus
  csrr t2, mepc
  csrr t3, mscratch
  //c->np = t4 = (sp==scratch ? 0(user) : 3(kernel)) = (xor(sp, scratch) != 0 ? 0(user) : 3(kernel))
  addi t4, sp, CONTEXT_SIZE
  xor  t5, t4, t3
  mv   t4, zero
  bnez t5, user_skip_add
  addi t4, t4, 3

user_skip_add:
  // support re-entry of CTE, mscratch=0
  csrw mscratch, zero

  STORE t0, OFFSET_CAUSE(sp)
  STORE t1, OFFSET_STATUS(sp)
  STORE t2, OFFSET_EPC(sp)
  STORE t3, OFFSET_SP(sp)
  STORE t4, OFFSET_NP(sp)

  # set mstatus.MPRV to pass difftest
  li a0, (1 << 17)
  or t1, t1, a0
  csrw mstatus, t1

  mv a0, sp
  jal __am_irq_handle
  # switch to new context
  mv sp, a0

  LOAD t0, OFFSET_CAUSE(sp)
  LOAD t1, OFFSET_STATUS(sp)
  LOAD t2, OFFSET_EPC(sp)
  LOAD t3, OFFSET_SP(sp)
  LOAD t4, OFFSET_NP(sp)
  csrw mcause, t0
  csrw mstatus, t1
  csrw mepc, t2
  
  //mscratch = (c->np == 0(user)) ? sp + CONEXT_SIZE : 0;
  bnez t4, restore_context
  addi t4, sp, CONTEXT_SIZE
  csrw mscratch, t4

restore_context:
  MAP(REGS, POP)
  LOAD sp, OFFSET_SP(sp)

  mret
