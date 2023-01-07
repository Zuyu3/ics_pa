#include <memory.h>

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
  return 0;
}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
