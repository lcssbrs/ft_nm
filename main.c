/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lseiberr <lseiberr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/19 16:49:46 by lseiberr          #+#    #+#             */
/*   Updated: 2024/12/20 12:39:28 by lseiberr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_nm.h"
#include <stdbool.h>

#define ELF64_SECTION_HEADER_SIZE 0x40

bool is_valid_elf(void *ptr, int *endian, int *bit)
{
  Elf64_Ehdr *header = (Elf64_Ehdr *)ptr;
    if (header->e_ident[0] == 0x7f && header->e_ident[1] == 'E' && header->e_ident[2] == 'L' && header->e_ident[3] == 'F')
        ft_printf("This is an ELF file\n");
    else
     {
        ft_printf("This is not an ELF file\n");
        return (false);
        }
    *endian = header->e_ident[EI_DATA];
    *bit = header->e_ident[EI_CLASS];
    if (*bit == ELFCLASS32)
        ft_printf("32 bits\n");
    else if (*bit == ELFCLASS64)
        ft_printf("64 bits\n");
    else
      {
        ft_printf("Invalid class\n");
        return (false);
        }
    return (true);
}

bool open_elf(char *filename, void **ptr)
    {
  struct stat buf;
      int fd;
  fd = open(filename, O_RDONLY);
    if (fd < 0)
    {
        ft_putstr_fd("Error: open failed\n", 2);
        return (false);
    }
    if (fstat(fd, &buf) < 0)
    {
        ft_putstr_fd("Error: fstat failed\n", 2);
        return (false);
    }
    if (buf.st_size < ELF64_SECTION_HEADER_SIZE)
    {
        ft_putstr_fd("Error: file too small\n", 2);
        return (false);
    }
    *ptr = mmap(0, buf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (ptr == MAP_FAILED)
    {
        ft_putstr_fd("Error: mmap failed\n", 2);
        return (false);
    }

    if (close(fd) < 0)
    {
        ft_putstr_fd("Error: close failed\n", 2);
        return (false);
    }
	return (true);
    }

void parse_symbol(void *ptr, Elf64_Shdr *section_headers, int i){

    Elf64_Sym *symbols = (Elf64_Sym *)(ptr + section_headers[i].sh_offset);
    size_t symbol_count = section_headers[i].sh_size / sizeof(Elf64_Sym);
    const char *sym_string_table = (const char *)(ptr + section_headers[section_headers[i].sh_link].sh_offset);
    for (size_t j = 0; j < symbol_count; j++)
    {
    Elf64_Sym *sym = &symbols[j];
    const char *sym_name = &sym_string_table[sym->st_name];
    unsigned char sym_bind = ELF64_ST_BIND(sym->st_info);
        if (sym_bind == STB_GLOBAL)
        {
            if (ELF32_ST_TYPE(sym->st_info) == STT_OBJECT)
                printf("                 O");
            else if (ELF32_ST_TYPE(sym->st_info) == STT_FUNC)
            {
                printf("%016lx ", sym->st_value);
                printf("T");
            }
            else if (ELF32_ST_TYPE(sym->st_info) == STT_SECTION)
                printf("                 S");
            else if (ELF32_ST_TYPE(sym->st_info) == STT_FILE)
                printf("                 T");
            else if (sym->st_value == 0)
                printf("                 U");
            else
                printf("                 A");
            printf(" %s\n", sym_name);
        }
    }
}

void parse_symbol32(void *ptr, Elf32_Shdr *section_headers, int i){

    Elf32_Sym *symbols = (Elf32_Sym *)(ptr + section_headers[i].sh_offset);
    size_t symbol_count = section_headers[i].sh_size / sizeof(Elf32_Sym);
    const char *sym_string_table = (const char *)(ptr + section_headers[section_headers[i].sh_link].sh_offset);
    for (size_t j = 0; j < symbol_count; j++)
    {
    Elf32_Sym *sym = &symbols[j];
    const char *sym_name = &sym_string_table[sym->st_name];
    unsigned char sym_bind = ELF32_ST_BIND(sym->st_info);
        if (sym_bind == STB_GLOBAL)
        {
            if (ELF32_ST_TYPE(sym->st_info) == STT_OBJECT)
                printf("                 O");
            else if (ELF32_ST_TYPE(sym->st_info) == STT_FUNC)
            {
                printf("%016x ", sym->st_value);
                printf("T");
            }
            else if (ELF32_ST_TYPE(sym->st_info) == STT_SECTION)
                printf("                 S");
            else if (ELF32_ST_TYPE(sym->st_info) == STT_FILE)
                printf("                 T");
            else if (sym->st_value == 0)
                printf("                 U");
            else
                printf("                 A");
            printf(" %s\n", sym_name);
        }
    }
}


int main(int ac, char **ag)
{
    char *filename;
	void *ptr;
    int bit = 0;
    int endian = 0;


    filename = "a.out";
    if (ac >= 2)
      {
        filename = ag[1];
      }
    if (!open_elf(filename, &ptr))
        return (1);
    if (!is_valid_elf(ptr, &endian, &bit))
    {
        return (1);
    }
	if (bit == ELFCLASS32)
    {
        Elf32_Ehdr *header = (Elf32_Ehdr *)ptr;
        Elf32_Shdr *section_headers = (Elf32_Shdr *)(ptr + header->e_shoff);

        for (int i = 0; i < header->e_shnum; i++)
        {
            if (section_headers[i].sh_type == SHT_SYMTAB)
            {
                parse_symbol32(ptr, section_headers, i);
            }
        }
    }
    else if (bit == ELFCLASS64)
    {
        Elf64_Ehdr *header = (Elf64_Ehdr *)ptr;
        Elf64_Shdr *section_headers = (Elf64_Shdr *)(ptr + header->e_shoff);

        for (int i = 0; i < header->e_shnum; i++)
        {
            if (section_headers[i].sh_type == SHT_SYMTAB)
            {
                parse_symbol(ptr, section_headers, i);
            }
        }
    }
}
