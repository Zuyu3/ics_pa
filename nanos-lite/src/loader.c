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
size_t ramdisk_write(const void *buf, size_t offset, size_t len);



static uintptr_t loader(PCB *pcb, const char *filename) {
  Elf_Ehdr elf_header;
  Elf_Phdr elf_prog_header;
  ramdisk_read(&elf_header, 0, sizeof(elf_header));
  assert(*(uint32_t *)elf_header.e_ident == 0x7f454c46);

  for(int i = 0; i < elf_header.e_phnum; i++) {
    ramdisk_read(&elf_prog_header, elf_header.e_phoff + elf_header.e_phentsize * i, elf_header.e_phentsize);
    printf("%u  %x  %x  %x  %u  %u\n\n", elf_prog_header.p_type, elf_prog_header.p_offset, elf_prog_header.p_vaddr, elf_prog_header.p_paddr, elf_prog_header.p_filesz, elf_prog_header.p_memsz);

  }  

  return 0;

}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

