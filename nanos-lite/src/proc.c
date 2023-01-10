#include <proc.h>
#include <fs.h>

#define MAX_NR_PROC 4
#define RUN_PCB0_PER 20

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;
static int pcb_index = 0;
static int schedule_time_slice_index = 0;

void naive_uload(PCB *pcb, const char *filename);
void context_kload(PCB *pcb, void (*entry)(void *), void *arg);
void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]);
char *const test_env[] = {"first=1", "second=1221", "end=0s", "h1h2h33", NULL};
char *const test_arg[] = {"/bin/exec-test", "0", "/bin/pal", "--skip", "end of args", NULL};

PCB *new_pcb() {
  //printf("alloc pcb[%d]\n", pcb_index);
  if(pcb_index >= MAX_NR_PROC)
    panic("Malloc too much pcb. No free PCB\n");

  pcb_index++;
  return &pcb[pcb_index - 1];
}

void switch_boot_pcb() {
  current = &pcb_boot;
}

int execve_load_ucontext(const char *pathname, char *const argv[], char *const envp[]) {
  if(fs_open(pathname, 0, 0) == -1) 
    return -2;

  context_uload(new_pcb(), pathname, argv, envp);
  switch_boot_pcb();
  return 0;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    if(j % 100 == 0)
    Log("Hello World from Nanos-lite with arg '%p' for the %dth time!", (uintptr_t)arg, j);
    j ++;
    yield();
  }
}

void init_proc() {
  
  context_kload(new_pcb(), hello_fun, (void *)0);

  //context_uload(new_pcb(), "/bin/pal", test_arg, NULL);
  //context_uload(new_pcb(), "/bin/hello", NULL, NULL);
  context_uload(new_pcb(), "/bin/nterm", NULL, NULL);

  switch_boot_pcb();

  Log("Initializing processes...");

  // load program here
  //naive_uload(NULL, "/bin/nterm");
}

Context* schedule(Context *prev) {
  // save the context pointer
  current->cp = prev;
  
  if(schedule_time_slice_index % RUN_PCB0_PER == 0) {
    current = &pcb[0];
    printf("schedule pcb[0] at slice[%d]\n", schedule_time_slice_index);
    schedule_time_slice_index = 0;
  }
  else {
    current = &pcb[pcb_index - 1];
  }
  schedule_time_slice_index++;
  //current = (current == &pcb[0] ? &pcb[pcb_index - 1] : &pcb[0]);
  //Log("schedule %d\n", (current - &pcb[0]));

  // then return the new context
  return current->cp;
}
