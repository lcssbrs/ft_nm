/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lseiberr <lseiberr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/19 16:49:46 by lseiberr          #+#    #+#             */
/*   Updated: 2024/12/20 13:02:21 by lseiberr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_nm.h"
#include <stdbool.h>

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <elf.h>

// Structure pour stocker les symboles et leurs informations
typedef struct s_symbol
{
    unsigned long address;  // Adresse du symbole
    char type;              // Type du symbole (U, T, etc.)
    char *name;             // Nom du symbole
} t_symbol;

// Fonction de comparaison pour trier les symboles par nom
int compare_symbols_by_name(const void *a, const void *b)
{
    t_symbol *symbol_a = (t_symbol *)a;
    t_symbol *symbol_b = (t_symbol *)b;

    return strcmp(symbol_a->name, symbol_b->name);  // Trie lexicographiquement par le nom
}

void print_symbol(t_symbol symbol)
{
    // Affichage de l'adresse
    if (symbol.address == 0)
        printf("                 "); // 16 espaces pour les symboles non définis
    else
        printf("%016lx ", symbol.address); // Afficher l'adresse sur 16 caractères

    // Affichage du type de symbole
    printf("%c ", symbol.type);

    // Affichage du nom du symbole
    printf("%s\n", symbol.name);
}

void print_symbol32(t_symbol symbol)
{
    // Affichage de l'adresse
    if (symbol.address == 0)
        printf("                 "); // 16 espaces pour les symboles non définis
    else
        printf("%08lx ", symbol.address); // Afficher l'adresse sur 16 caractères

    // Affichage du type de symbole
    printf("%c ", symbol.type);

    // Affichage du nom du symbole
    printf("%s\n", symbol.name);
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

        // Ignorer les symboles sans nom
        if (sym_name == NULL || sym->st_name == 0)
            continue;

        // Ignorer les symboles de type STT_FILE (symboles de type "a")
        if (ELF64_ST_TYPE(sym->st_info) == STT_FILE)
            continue;

        // Ignorer les symboles dont le nom commence par $ (comme $d, $x, etc.)
        if (sym_name[0] == '$')
            continue;

        unsigned char sym_bind = ELF64_ST_BIND(sym->st_info);
        unsigned char sym_type = ELF64_ST_TYPE(sym->st_info);
        t_symbol symbol;

        // Affecter l'adresse du symbole
        symbol.address = sym->st_value;

        // Déterminer le type du symbole
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
                symbol.type = 'a';  // Symbole de type STT_FILE (qu'on veut ignorer)
        }

        // Si c'est un symbole de type 'a', on l'ignore
        if (symbol.type == 'a')
            continue;

        // Copier le nom du symbole
        symbol.name = strdup(sym_name);

        // Ajouter le symbole à notre tableau
        symbols_array[symbol_index++] = symbol;
    }

    // Trier les symboles par nom (lexicographiquement)
    qsort(symbols_array, symbol_index, sizeof(t_symbol), compare_symbols_by_name);

    // Afficher les symboles triés
    for (int i = 0; i < symbol_index; i++)
    {
        print_symbol(symbols_array[i]);
        free(symbols_array[i].name);
    }

    // Libérer la mémoire allouée pour le tableau des symboles
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

        // Ignorer les symboles sans nom
        if (sym_name == NULL || sym->st_name == 0)
            continue;

        // Ignorer les symboles de type STT_FILE (symboles de type "a")
        if (ELF32_ST_TYPE(sym->st_info) == STT_FILE)
            continue;

        // Ignorer les symboles dont le nom commence par $ (comme $d, $x, etc.)
        if (sym_name[0] == '$')
            continue;

        unsigned char sym_bind = ELF32_ST_BIND(sym->st_info);
        unsigned char sym_type = ELF32_ST_TYPE(sym->st_info);
        t_symbol symbol;

        // Affecter l'adresse du symbole
        symbol.address = sym->st_value;

        // Déterminer le type du symbole
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
                symbol.type = 'a';  // Symbole de type STT_FILE (qu'on veut ignorer)
        }

        // Si c'est un symbole de type 'a', on l'ignore
        if (symbol.type == 'a')
            continue;

        // Copier le nom du symbole
        symbol.name = strdup(sym_name);

        // Ajouter le symbole à notre tableau
        symbols_array[symbol_index++] = symbol;
    }

    // Trier les symboles par nom (lexicographiquement)
    qsort(symbols_array, symbol_index, sizeof(t_symbol), compare_symbols_by_name);

    // Afficher les symboles triés
    for (int i = 0; i < symbol_index; i++)
    {
        print_symbol32(symbols_array[i]);
        free(symbols_array[i].name);
    }

    // Libérer la mémoire allouée pour le tableau des symboles
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
