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
  printf("%x   %x\n", &pcb->cp, pcb);
  karea.start = &pcb->cp;
  karea.end = karea.start + STACK_SIZE;
  printf("area is:(%x, %x)\nentry address: %x\n", karea.start, karea.end, entry);
  pcb->cp = kcontext(karea, entry, arg);
}

void context_uload(PCB *pcb, const char* filename) {
  uintptr_t entry = loader(pcb, filename);
  Area karea;
  karea.start = &pcb->cp;
  karea.end = &pcb->cp + STACK_SIZE;

  pcb->cp = ucontext(NULL, karea, (void *)entry);
  pcb->cp->GPRx = (uintptr_t)heap.end;
}