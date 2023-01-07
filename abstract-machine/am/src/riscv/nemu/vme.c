#include <am.h>
#include <nemu.h>
#include <klib.h>
#include <riscv/riscv.h>

static AddrSpace kas = {};
static void* (*pgalloc_usr)(int) = NULL;
static void (*pgfree_usr)(void*) = NULL;
static int vme_enable = 0;

static Area segments[] = {      // Kernel memory mappings
  NEMU_PADDR_SPACE
};

#define USER_SPACE RANGE(0x40000000, 0x80000000)

static inline void set_satp(void *pdir) {
  uintptr_t mode = 1ul << (__riscv_xlen - 1);
  asm volatile("csrw satp, %0" : : "r"(mode | ((uintptr_t)pdir >> 12)));
}

static inline uintptr_t get_satp() {
  uintptr_t satp;
  asm volatile("csrr %0, satp" : "=r"(satp));
  return satp << 12;
}

bool vme_init(void* (*pgalloc_f)(int), void (*pgfree_f)(void*)) {
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  kas.ptr = pgalloc_f(PGSIZE);

  int i;
  for (i = 0; i < LENGTH(segments); i ++) {
    void *va = segments[i].start;
    for (; va < segments[i].end; va += PGSIZE) {
      map(&kas, va, va, 0);
    }
  }

  set_satp(kas.ptr);
  vme_enable = 1;

  return true;
}

void protect(AddrSpace *as) {
  PTE *updir = (PTE*)(pgalloc_usr(PGSIZE));
  as->ptr = updir;
  as->area = USER_SPACE;
  as->pgsize = PGSIZE;
  // map kernel space
  memcpy(updir, kas.ptr, PGSIZE);
}

void unprotect(AddrSpace *as) {
}

void __am_get_cur_as(Context *c) {
  c->pdir = (vme_enable ? (void *)get_satp() : NULL);
}

void __am_switch(Context *c) {
  if (vme_enable && c->pdir != NULL) {
    set_satp(c->pdir);
  }
}

void map(AddrSpace *as, void *va, void *pa, int prot) {
  // get_satp() has already let PPN multiple 4096( << 12)
  // page_table1 = PPN * 4096 + va[31:22] * 4
  // page_table0 = *page_table1.PPN * 4096 + va[21:12] * 4
  uintptr_t vpn1 = (uintptr_t)va >> 22, vpn0 = ((uintptr_t)va >> 12) & 0x3ff, offset = (uintptr_t)va & 0xfff;
  PTE *page_table1 = (PTE *)(get_satp() + vpn1 * 4);

  if(!(*page_table1 & PTE_V)) {
    // page_table0 map is not valid, alloc a page for it.
    uintptr_t page_alloced = (uintptr_t)pgalloc_usr(PGSIZE);
    *page_table1 = 0;
    // page_alloced = 0x?????000, page_alloced >> 12 << 10 = page_alloced >> 2
    *page_table1 = *page_table1 | page_alloced >> 2 | PTE_V;
    assert(*page_table1 >> 10 << 12 == page_alloced);
  }

  PTE *page_table0 = (void *)((*page_table1 >> 10 << 12) + vpn0 * 4);
  *page_table0 = ((uintptr_t)pa - offset) >> 12 << 10 | PTE_V | PTE_R | PTE_W | PTE_X;
  assert((*page_table0 >> 10 << 12) + offset == (uintptr_t)pa);
}

Context *ucontext(AddrSpace *as, Area kstack, void *entry) {
  Context *ctp = kstack.end - sizeof(Context);
  ctp->mepc = (uintptr_t)entry;
  ctp->mstatus = 0x1800;
  ctp->gpr[0] = 0;
  //set stack pointer sp when load in
  return ctp;
}
