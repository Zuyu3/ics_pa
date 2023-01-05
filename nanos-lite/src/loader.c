#include <proc.h>
#include <elf.h>
#include<fs.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif


#if defined(__ISA_AM_NATIVE__)
# define EXPECT_TYPE EM_X86_64
#elif defined(__ISA_RISCV32__)
# define EXPECT_TYPE EM_RISCV
#else
# error Unsupported ISA
#endif

Context *kcontext(Area kstack, void (*entry)(void *), void *arg);
Context *ucontext(AddrSpace *as, Area kstack, void *entry);

static uintptr_t loader(PCB *pcb, const char *filename) {
  
  int file_id = fs_open(filename, 0, 0);
  //printf("Load file: %s\nFile id is %d\n", filename, file_id);
  
  
  Elf_Ehdr elf_header;
  Elf_Phdr prog_header;
  fs_read(file_id, &elf_header, sizeof(elf_header));

  if(*(uint32_t *)elf_header.e_ident != 0x464c457f) {
    panic("Magic number Error.\n");
  }
  if(elf_header.e_machine != EXPECT_TYPE) {
    panic("ISA error. File's ISA is different from OS.\n");
  }

  for(int i = 0; i < elf_header.e_phnum; i++) {
    fs_lseek(file_id, elf_header.e_phoff + elf_header.e_phentsize * i, SEEK_SET);
    fs_read(file_id, &prog_header, elf_header.e_phentsize);

    if(prog_header.p_type != PT_LOAD)
      continue;
    
    fs_lseek(file_id, prog_header.p_offset, SEEK_SET);
    fs_read(file_id, (void *)prog_header.p_vaddr, prog_header.p_filesz);

    if(prog_header.p_flags & 0x2) {
      memset((void *)prog_header.p_vaddr + prog_header.p_filesz, 0, prog_header.p_memsz - prog_header.p_filesz);
    }
  }
  
  fs_close(file_id);
  return elf_header.e_entry;

}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void (*entry)(void *), void *arg) {
  Area karea;
  karea.start = &pcb->cp;
  karea.end = karea.start + STACK_SIZE;
  pcb->cp = kcontext(karea, entry, arg);
}

void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]) {
  int argc = 0, envc = 0;
  void *stack_start = heap.end;

  if(argv) {
    while(argv[argc])
      argc++;
  }
  if(envp) {
    while(envp[envc])
      envc++;
  }

  char *arg_pointer[argc], *env_pointer[envc];

  for(int i = envc - 1; i >= 0; i--) {
    stack_start -= (strlen(envp[i]) + 1);
    env_pointer[i] = stack_start;
    strcpy(stack_start, envp[i]);
    printf("env[%d] at %p: %s, len=%d\n", i, env_pointer[i], env_pointer[i], strlen(envp[i]) + 1);
  }

  for(int i = argc - 1; i >= 0; i--) {
    stack_start -= (strlen(argv[i]) + 1);
    arg_pointer[i] = stack_start;
    strcpy(stack_start, argv[i]);
    printf("arg[%d] at %p: %s\n", i, arg_pointer[i], arg_pointer[i]);
  }

  char **pointer_stack = (char **)stack_start;

  //stack align
  int align_const = (EXPECT_TYPE == EM_RISCV ? 4: 8);
  align_const = (uintptr_t)pointer_stack % align_const;
  pointer_stack = (char **)((uintptr_t)pointer_stack - align_const);

  pointer_stack--;
  *pointer_stack = NULL;
  for(int i = envc - 1; i >= 0; i--) {
    pointer_stack--;
    *pointer_stack = env_pointer[i];
  }
  stack_start = (void *)pointer_stack;


  for(int i = 0; i <= envc; i++) {
    printf("env [%d]: %p\n", i, pointer_stack[i]);
  }

  pointer_stack--;
  *pointer_stack = NULL;
  for(int i = argc - 1; i >= 0; i--) {
    pointer_stack--;
    *pointer_stack = arg_pointer[i];
  }

  for(int i = 0; i <= argc; i++) {
    printf("arg [%d]: %p\n", i, pointer_stack[i]);
  }

  --pointer_stack;
  *pointer_stack = stack_start;
  --pointer_stack;
  *pointer_stack = (void *)(pointer_stack + 2);
  --pointer_stack;
  *(int *)pointer_stack = argc;
  printf("%d %p %p\n", *(int *)pointer_stack, *(pointer_stack + 1), *(pointer_stack + 2));

  int test_argc = *(int *)pointer_stack;
  void *test_argv = *(void **)pointer_stack, *test_envp = *(void **)pointer_stack;
  printf("%d  %p  %p\n", test_argc, test_argv, test_envp);
  assert(0);



  uintptr_t entry = loader(pcb, filename);
  Area karea;
  karea.start = &pcb->cp;
  karea.end = &pcb->cp + STACK_SIZE;
  pcb->cp = ucontext(NULL, karea, (void *)entry);

  pcb->cp->GPRx = (uintptr_t) pointer_stack;
}