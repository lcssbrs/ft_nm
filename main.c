/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lseiberr <lseiberr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/19 16:49:46 by lseiberr          #+#    #+#             */
/*   Updated: 2024/12/20 13:52:25 by lseiberr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_nm.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <elf.h>

#define ELF64_SECTION_HEADER_SIZE 0x40

bool is_valid_elf(void *ptr, int *endian, int *bit)
{
  Elf64_Ehdr *header = (Elf64_Ehdr *)ptr;
    if (header->e_ident[0] == 0x7f && header->e_ident[1] == 'E' && header->e_ident[2] == 'L' && header->e_ident[3] == 'F')
        ft_printf("");
    else
     {
        ft_printf("This is not an ELF file\n");
        return (false);
        }
    *endian = header->e_ident[EI_DATA];
    *bit = header->e_ident[EI_CLASS];
    if (*bit == ELFCLASS32)
        ft_printf("");
    else if (*bit == ELFCLASS64)
        ft_printf("");
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

// Structure pour stocker les symboles et leurs informations
typedef struct s_symbol
{
    unsigned long address;  // Adresse du symbole
    char type;              // Type du symbole (U, T, etc.)
    char *name;             // Nom du symbole
} t_symbol;

int compare_symbols_by_name(const void *a, const void *b)
{
    t_symbol *symbol_a = (t_symbol *)a;
    t_symbol *symbol_b = (t_symbol *)b;

    return ft_strncmp(symbol_a->name, symbol_b->name, 100);
}

void	ft_puthex32(long long num)
{
	char	*base;

	base = "0123456789abcdef";  // Utilisation de l'alphabet hexadécimal en minuscules
	if (num >= 16)
	{
		ft_puthex32(num / 16);
		ft_puthex32(num % 16);
	}
	else
	{
		write(1, &base[num], 1);
	}
}

int	ft_printhex32(long long num)
{
	int len;
	int width = 8;  // Largeur totale à atteindre (16 caractères)
	int padding = 0;

	// Afficher le préfixe "0x"
	write(1, "0x", 2);

	// Si le numéro est zéro, afficher "0x0" (un seul zéro)
	if (num == 0)
	{
		write(1, "0", 1);
		return (3);  // "0x" + "0" = 3 caractères
	}

	// Calculer la longueur de l'adresse en hexadécimal
	len = ft_hex_len(num);
	// Calculer combien de zéros ajouter pour que l'affichage ait une largeur de 16 caractères
	padding = width - 2 - len;  // Le préfixe "0x" compte déjà pour 2 caractères

	// Ajouter les zéros nécessaires pour compléter la largeur
	while (padding > 0)
	{
		write(1, "0", 1);
		padding--;
	}

	// Afficher l'adresse en hexadécimal
	ft_puthex32(num);

	// Retourner la longueur totale : "0x" + adresse en hex (avec padding)
	return (len + 2 + (width - 2 - len));  // +2 pour "0x" et + padding pour les zéros
}

void print_symbol(t_symbol symbol)
{
    if (symbol.address == 0)
        ft_printf("                 ");
    else
        ft_printf("%x ", symbol.address);
    ft_printf("%c ", symbol.type);
    ft_printf("%s\n", symbol.name);
}

void print_symbol32(t_symbol symbol)
{
    if (symbol.address == 0)
        ft_printf("         ");
    else
        ft_printhex32(symbol.address);
    ft_printf("%c ", symbol.type);
    ft_printf("%s\n", symbol.name);
}

void ft_qsort(void *base, size_t n_items, size_t size, int (*compar)(const void *, const void *))
{
    if (n_items < 2)
        return;

    char *arr = (char *)base;
    char *pivot = arr + (n_items / 2) * size;
    size_t i = 0;
    size_t j = n_items - 1;

    // Partitionnement autour du pivot
    while (i <= j)
    {
        while (compar(arr + i * size, pivot) < 0)
            i++;
        while (compar(arr + j * size, pivot) > 0)
            j--;

        if (i <= j)
        {
            // Échanger les éléments
            char temp[size];
            memcpy(temp, arr + i * size, size);
            memcpy(arr + i * size, arr + j * size, size);
            memcpy(arr + j * size, temp, size);
            i++;
            j--;
        }
    }

    // Tri récursif des sous-tableaux
    if (j > 0)
        ft_qsort(base, j + 1, size, compar);
    if (i < n_items)
        ft_qsort(arr + i * size, n_items - i, size, compar);
}

void parse_symbol(void *ptr, Elf64_Shdr *section_headers, int i)
{
    Elf64_Sym *symbols = (Elf64_Sym *)(ptr + section_headers[i].sh_offset);
    size_t symbol_count = section_headers[i].sh_size / sizeof(Elf64_Sym);
    const char *sym_string_table = (const char *)(ptr + section_headers[section_headers[i].sh_link].sh_offset);

    t_symbol *symbols_array = malloc(symbol_count * sizeof(t_symbol));
    int symbol_index = 0;

    for (size_t j = 0; j < symbol_count; j++)
    {
        Elf64_Sym *sym = &symbols[j];
        const char *sym_name = &sym_string_table[sym->st_name];
        if (sym_name == NULL || sym->st_name == 0)
            continue;
        if (ELF64_ST_TYPE(sym->st_info) == STT_FILE)
            continue;
        if (sym_name[0] == '$')
            continue;

        unsigned char sym_bind = ELF64_ST_BIND(sym->st_info);
        unsigned char sym_type = ELF64_ST_TYPE(sym->st_info);
        t_symbol symbol;
        symbol.address = sym->st_value;

        if (sym_bind == STB_GLOBAL)
        {
            if (sym_type == STT_OBJECT)
                symbol.type = 'D';  // Données globales
            else if (sym_type == STT_FUNC)
                symbol.type = 'T';  // Fonction globale
            else if (sym_type == STT_SECTION)
                symbol.type = 'S';  // Section
            else if (sym->st_value == 0)
                symbol.type = 'U';  // Non défini
            else
                symbol.type = 'A';  // Autre type
        }
        else if (sym_bind == STB_LOCAL)
        {
            if (sym_type == STT_FUNC)
                symbol.type = 't';  // Fonction locale
            else if (sym_type == STT_OBJECT)
                symbol.type = 'd';  // Donnée locale
            else if (sym_type == STT_SECTION)
                symbol.type = 's';  // Section locale
            else
                symbol.type = 'a';
        }
        if (symbol.type == 'a')
            continue;
        symbol.name = ft_strdup((char *)sym_name);
        symbols_array[symbol_index++] = symbol;
    }
    ft_qsort(symbols_array, symbol_index, sizeof(t_symbol), compare_symbols_by_name);
    for (int i = 0; i < symbol_index; i++)
    {
        print_symbol(symbols_array[i]);
        free(symbols_array[i].name);
    }
    free(symbols_array);
}


void parse_symbol32(void *ptr, Elf32_Shdr *section_headers, int i)
{
    Elf32_Sym *symbols = (Elf32_Sym *)(ptr + section_headers[i].sh_offset);
    size_t symbol_count = section_headers[i].sh_size / sizeof(Elf32_Sym);
    const char *sym_string_table = (const char *)(ptr + section_headers[section_headers[i].sh_link].sh_offset);

    t_symbol *symbols_array = malloc(symbol_count * sizeof(t_symbol));
    int symbol_index = 0;

    for (size_t j = 0; j < symbol_count; j++)
    {
        Elf32_Sym *sym = &symbols[j];
        const char *sym_name = &sym_string_table[sym->st_name];
        if (sym_name == NULL || sym->st_name == 0)
            continue;
        if (ELF32_ST_TYPE(sym->st_info) == STT_FILE)
            continue;
        if (sym_name[0] == '$')
            continue;

        unsigned char sym_bind = ELF32_ST_BIND(sym->st_info);
        unsigned char sym_type = ELF32_ST_TYPE(sym->st_info);
        t_symbol symbol;
        symbol.address = sym->st_value;

        // Déterminer le type du symbole
        if (sym_bind == STB_GLOBAL)
        {
            if (sym_type == STT_OBJECT)
                symbol.type = 'D';
            else if (sym_type == STT_FUNC)
                symbol.type = 'T';
            else if (sym_type == STT_SECTION)
                symbol.type = 'S';
            else if (sym->st_value == 0)
                symbol.type = 'U';
            else
                symbol.type = 'A';
        }
        else if (sym_bind == STB_LOCAL)
        {
            if (sym_type == STT_FUNC)
                symbol.type = 't';
            else if (sym_type == STT_OBJECT)
                symbol.type = 'd';
            else if (sym_type == STT_SECTION)
                symbol.type = 's';
            else
                symbol.type = 'a';
        }
        if (symbol.type == 'a')
            continue;
        symbol.name = ft_strdup((char*)sym_name);
        symbols_array[symbol_index++] = symbol;
    }
    ft_qsort(symbols_array, symbol_index, sizeof(t_symbol), compare_symbols_by_name);
    for (int i = 0; i < symbol_index; i++)
    {
        print_symbol32(symbols_array[i]);
        free(symbols_array[i].name);
    }
    free(symbols_array);
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
