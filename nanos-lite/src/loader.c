#include <proc.h>
#include <elf.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

size_t ramdisk_read(void *buf, size_t offset, size_t len);
//size_t ramdisk_write(const void *buf, size_t offset, size_t len);


#if defined(__ISA_AM_NATIVE__)
# define EXPECT_TYPE EM_X86_64
#elif defined(__ISA_RISCV32__)
# define EXPECT_TYPE EM_RISCV
#else
# error Unsupported ISA
#endif


static uintptr_t loader(PCB *pcb, const char *filename) {
  #ifdef __ISA_
  printf("%d", 1)
  #endif
  Elf_Ehdr elf_header;
  Elf_Phdr prog_header;
  ramdisk_read(&elf_header, 0, sizeof(elf_header));

  assert(*(uint32_t *)elf_header.e_ident == 0x464c457f);
  printf("%d  %d\n", elf_header.e_machine, EXPECT_TYPE);
  assert(elf_header.e_machine == EXPECT_TYPE);

  for(int i = 0; i < elf_header.e_phnum; i++) {
    ramdisk_read(&prog_header, elf_header.e_phoff + elf_header.e_phentsize * i, elf_header.e_phentsize);
    if(prog_header.p_type != PT_LOAD)
      continue;
    
    ramdisk_read((void *)prog_header.p_vaddr, prog_header.p_offset, prog_header.p_filesz);
    if(prog_header.p_flags & 0x2) {
      memset((void *)prog_header.p_vaddr + prog_header.p_filesz, 0, prog_header.p_memsz - prog_header.p_filesz);
    }
  }
  

  return elf_header.e_entry;

}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

