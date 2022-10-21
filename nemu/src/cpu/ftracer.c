#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<common.h>
#define FTRACER_SIZE 2000

char elf_path[150];
FILE *fp = NULL;
unsigned int current_offset = 0;

typedef struct {
    unsigned int e_shoff;
    unsigned int e_shentsize, e_shnum, e_shstrndx;
} ElfHeader;


typedef struct {
    unsigned int sh_name_offset, sh_offset, sh_size;
    char sh_name[50];
} Section_Header_Data;

typedef struct
{
    unsigned int name_offset, func_start_addr, func_end_addr;
    char func_name[100];
} Function_Table;




ElfHeader *elf_header = NULL;
Section_Header_Data *sh_data = NULL;
Function_Table *func_table = NULL;
Function_Table *func_tracer_buf[FTRACER_SIZE];
int func_table_size = 0, func_tracer_index = 0;

void reset_fp_offset() {
    rewind(fp);
    current_offset = 0;
}

void load_data_from_elf(unsigned int offset, int size, void *pointer) {
    if(offset < current_offset)
        reset_fp_offset();

    for(int i = current_offset; i < offset; i++) {
        fgetc(fp);
    }
    for(int i = 0; i < size; i++) {
        *(char *)(pointer + i) = fgetc(fp);
    }
    current_offset = offset + size;
}

void load_string_from_elf(unsigned int offset, char *pointer) {
    if(offset < current_offset)
        reset_fp_offset();
    
    for(int i = current_offset; i < offset; i++) {
        fgetc(fp);
    }
    
    int counter = 0;
    do {
        pointer[counter] = fgetc(fp);
    } while(pointer[counter++] != '\0');

    current_offset = offset + counter;

}

void load_elf_header() {
    elf_header = (ElfHeader *)malloc(sizeof(ElfHeader));
    memset(elf_header, 0, sizeof(ElfHeader));
    load_data_from_elf(32, 4, &elf_header->e_shoff);
    load_data_from_elf(46, 2, &elf_header->e_shentsize);
    load_data_from_elf(48, 2, &elf_header->e_shnum);
    load_data_from_elf(50, 2, &elf_header->e_shstrndx);
}

void load_section_header() {
    sh_data = (Section_Header_Data *)malloc(sizeof(Section_Header_Data) * elf_header->e_shnum);
    for(int i = 0; i < elf_header->e_shnum; i++) {
        load_data_from_elf(elf_header->e_shoff + elf_header->e_shentsize * i     , 4, &sh_data[i].sh_name_offset);
        load_data_from_elf(elf_header->e_shoff + elf_header->e_shentsize * i + 16, 4, &sh_data[i].sh_offset);
        load_data_from_elf(elf_header->e_shoff + elf_header->e_shentsize * i + 20, 4, &sh_data[i].sh_size);
    }

    for(int i = 0; i < elf_header->e_shnum; i++) {
        load_string_from_elf(sh_data[elf_header->e_shstrndx].sh_offset + sh_data[i].sh_name_offset, sh_data[i].sh_name);
    }
}

void resize_function_table(unsigned int new_size) {
    Function_Table *temp = (Function_Table *)malloc(sizeof(Function_Table) * new_size);
    for(int i = 0; i < func_table_size; i++) {
        memcpy(&temp[i], &func_table[i], sizeof(Function_Table));
    }
    free(func_table);
    func_table = temp;
}

void load_func_symtab_data(Section_Header_Data *symtab_section_header) {
    unsigned int sh_part_start = symtab_section_header->sh_offset;

    unsigned int temp_name_offset, temp_start_addr, temp_size;
    unsigned char temp_sym_type;
    unsigned int ft_size = 50;

    resize_function_table(ft_size);

    for(int i = 0; i < symtab_section_header->sh_size / 16; i++) {
        load_data_from_elf(sh_part_start     , 4, &temp_name_offset);
        load_data_from_elf(sh_part_start + 4 , 4, &temp_start_addr);
        load_data_from_elf(sh_part_start + 8 , 4, &temp_size);
        load_data_from_elf(sh_part_start + 12, 1, &temp_sym_type);
        temp_sym_type %= 16;
        sh_part_start += 16;

        if(temp_sym_type == 2) {
            func_table[func_table_size].name_offset = temp_name_offset;
            func_table[func_table_size].func_start_addr = temp_start_addr;
            func_table[func_table_size].func_end_addr = temp_start_addr + temp_size;
            func_table_size++;
            if(func_table_size == ft_size) {
                ft_size *= 2;
                resize_function_table(ft_size);
            }
        }
    }
}

void load_func_strtab_name(unsigned int strtab_start_addr) {
    for(int i = 0; i < func_table_size; i++) {
        load_string_from_elf(strtab_start_addr + func_table[i].name_offset, func_table[i].func_name);
    }
}

void init_func_table(char *bin_path, char *nemu_log_path) {
    if(strlen(bin_path) >= 150) {
        printf("elf_path_name_overflow\n");
        exit(1);
    }
    strcpy(elf_path, bin_path);
    strcpy(&elf_path[strlen(elf_path) - 3], "elf");

    fp = fopen(elf_path, "r");
    reset_fp_offset();
    load_elf_header(elf_header);
    //printf("%u\n%u\n%u\n%u\n", elf_header->e_shoff, elf_header->e_shentsize, elf_header->e_shnum, elf_header->e_shstrndx);

    load_section_header();
    //for(int i = 0; i < elf_header->e_shnum; i++)  printf("%-10x  %-10x  %-10x %s\n", sh_data[i].sh_name_offset, sh_data[i].sh_offset, sh_data[i].sh_size, sh_data[i].sh_name);

    for(int i = 0; i < elf_header->e_shnum; i++) {
        if(!strcmp(sh_data[i].sh_name, ".symtab")) {
            load_func_symtab_data(&sh_data[i]);
        }
        else if(!strcmp(sh_data[i].sh_name, ".strtab")) {
            load_func_strtab_name(sh_data[i].sh_offset);
        }
    }


    free(elf_header);
    free(sh_data); 

    /*for(int i = 0; i < func_table_size; i++)
      printf("%d  %x %x %s\n", i, func_table[i].func_start_addr, func_table[i].func_end_addr, func_table[i].func_name);
    printf("%d\n", func_table_size);
    */

    strcpy(elf_path, nemu_log_path);
    strcpy(&elf_path[strlen(elf_path) - 12], "ftracer-log.txt");
    fp = fopen(elf_path, "w");
    fflush(fp);
}

void check_func_log(vaddr_t target_addr, vaddr_t curr_addr) {
    #ifndef CONFIG_FTRACE
        return;
    #endif

    //printf("%x %x\n", curr_addr, target_addr);
    for(int i = func_tracer_index - 1; i >= 0; i--) 
        if(target_addr > func_tracer_buf[i]->func_start_addr && target_addr <= func_tracer_buf[i]->func_end_addr) {
            fprintf(fp, "0x%08x:", curr_addr);

            for(int k = 0; k < func_tracer_index; k++)
                fprintf(fp, "   ");
                    
            
            for(int k = 0; k < func_table_size; k++) {
                if(curr_addr >= func_table[k].func_start_addr && curr_addr < func_table[k].func_end_addr) {
                    fprintf(fp, " ret  [%s]\n", func_table[k].func_name);
                }
            }

            func_tracer_index = i;
            return;
        }

    for(int i = 0; i < func_table_size; i++) {
        if(target_addr == func_table[i].func_start_addr) {
            func_tracer_buf[func_tracer_index++] = &func_table[i];
            fprintf(fp, "0x%08x:", curr_addr);
            for(int i = 0; i < func_tracer_index; i++)
                fprintf(fp, "   ");
            fprintf(fp, " call [%s @0x%08x]\n", func_table[i].func_name, target_addr);
            return;
        }

    }

}