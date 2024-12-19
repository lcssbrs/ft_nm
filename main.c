/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lseiberr <lseiberr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/19 16:49:46 by lseiberr          #+#    #+#             */
/*   Updated: 2024/12/19 16:51:47 by lseiberr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_nm.h"
#define ELF64_SECTION_HEADER_SIZE 0x40

int main(int ac, char **ag)
{
    char *filename;
    int fd;
    struct stat buf;

    filename = "a.out";
    if (ac >= 2)
      {
        filename = ag[1];
      }
	fd = open(filename, O_RDONLY);
    if (fd < 0)
    {
        ft_putstr_fd("Error: open failed\n", 2);
        return (1);
    }
    if (fstat(fd, &buf) < 0)
    {
        ft_putstr_fd("Error: fstat failed\n", 2);
        return (1);
    }
    if (buf.st_size < ELF64_SECTION_HEADER_SIZE)
    {
        ft_putstr_fd("Error: file too small\n", 2);
        return (1);
    }
    void *ptr = mmap(0, buf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (ptr == MAP_FAILED)
    {
        ft_putstr_fd("Error: mmap failed\n", 2);
        return (1);
    }

    if (close(fd) < 0)
    {
        ft_putstr_fd("Error: close failed\n", 2);
        return (1);
    }
    Elf64_Ehdr *header = (Elf64_Ehdr *)ptr;
    if (header->e_ident[0] == 0x7f && header->e_ident[1] == 'E' && header->e_ident[2] == 'L' && header->e_ident[3] == 'F')
    {
        ft_printf("This is an ELF file\n");
    }
    else
    {
        ft_printf("This is not an ELF file\n");
    }
    int bit = header->e_ident[EI_CLASS];
    if (bit == ELFCLASS32)
    {
        ft_printf("32 bits\n");
        Elf32_Ehdr *newheader = (Elf32_Ehdr *)ptr;
        Elf32_Shdr *section = (Elf32_Shdr *)(ptr + newheader->e_shoff);
        printf("section header offset: %x\n", section->sh_offset);
    }
    else if (bit == ELFCLASS64)
    {
        ft_printf("64 bits\n");
    }
    else
    {
        ft_printf("Invalid class\n");
    }
    //Elf32_Shdr *section = (Elf32_Shdr *)(ptr + header->e_shoff);
    int endian = header->e_ident[EI_DATA];
    if (endian == ELFDATA2LSB)
    {
        ft_printf("Little endian\n");
    }
    else if (endian == ELFDATA2MSB)
    {
        ft_printf("Big endian\n");
    }
    else
    {
        ft_printf("Invalid endian\n");
    }
    //ft_printf("section header offset: %x\n", section->sh_offset);
}
