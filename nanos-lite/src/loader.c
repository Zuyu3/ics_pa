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
  ramdisk_read(&elf_header, 0, sizeof(elf_header));
  for(int i = 0; i < sizeof(elf_header); i++) {
    printf("%x %x %x %x %x\n", elf_header.e_machine, elf_header.e_entry, elf_header.e_phoff, elf_header.e_phentsize, elf_header.e_phnum);
  }
  printf("\n");
  return 0;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

