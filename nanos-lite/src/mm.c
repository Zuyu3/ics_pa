#include <memory.h>
#include <proc.h>

static void *pf = NULL;

void* new_page(size_t nr_page) {
  void *res = pf;
  pf += (nr_page * 4096);
  return res;
}

#ifdef HAS_VME
static void* pg_alloc(int n) {
  assert(n % 4096 == 0);
  int nr_page = n / 4096;
  void *pages_alloced = new_page(nr_page);
  memset(pages_alloced, 0, n);
  return pages_alloced;
}
#endif

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t brk) {
  uintptr_t max_brk = current->max_brk;
  int page_num = (brk >> 12) - (max_brk >> 12);
  printf("brk from %p to %p, alloc %d pages\n", max_brk, brk, page_num);
  if(brk < max_brk)
    return 0;
  
  void *page_alloced = new_page(page_num);
  for(int i = 0; i < page_num; i++) {
    map(&current->as, (void *)((max_brk & ~0xfff) + 4096 * (i + 1)), (void *)(page_alloced + 4096 * i), MMAP_READ | MMAP_WRITE);
  }
  current->max_brk = (brk & (~0xfff)) + 4096;
  return 0;
}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
