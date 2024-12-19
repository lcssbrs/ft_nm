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

int main()
{
    int fd;  // Descripteur de fichier
    const char *nom_fichier = "main.o";  // Nom du fichier à ouvrir
    struct stat sb;  // Pour obtenir la taille du fichier
    void *map;  // Pointeur pour le mappage en mémoire

    // Ouvrir le fichier en lecture seule
    fd = open(nom_fichier, O_RDONLY);
    if (fd == -1)
    {
        perror("Erreur d'ouverture du fichier");
        return 1;
    }

    // Obtenir les informations sur le fichier pour connaître sa taille
    if (fstat(fd, &sb) == -1)
    {
        perror("Erreur de récupération des informations sur le fichier");
        close(fd);
        return 1;
    }

    // Mapper le fichier en mémoire
    map = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (map == MAP_FAILED)
    {
        perror("Erreur de mappage du fichier");
        close(fd);
        return 1;
    }

    // Maintenant, tu peux accéder au contenu du fichier via le pointeur `map`
    // Exemple : afficher les premiers octets en hexadécimal
    for (off_t i = 0; i < sb.st_size; i++)
{
    printf("%02x ", ((unsigned char *)map)[i]);
}
printf("\n");
    printf("\n");
	Elf64_Ehdr *elf_header = (Elf64_Ehdr *)map;
    Elf64_Shdr *section_header = (Elf64_Shdr *)(map + elf_header->e_shoff);
    if(elf_header->e_type == 0x7f &&
       elf_header->e_ident[1] == 'E' &&
       elf_header->e_ident[2] == 'L' &&
       elf_header->e_ident[3] == 'F')
      {
      ft_printf("C'est un fichier ELF\n");
      }
     else
        {
        ft_printf("Ce n'est pas un fichier ELF\n");
        }
    printf("e_ident: ");
for (int i = 0; i < EI_NIDENT; i++) {
    printf("%02x ", elf_header->e_ident[i]);
}
    printf("e_type: %llu\n", section_header->sh_addr);
    // Démappage de la mémoire et fermeture du fichier
    if (munmap(map, sb.st_size) == -1)
    {
        perror("Erreur de démappage du fichier");
    }
    close(fd);
    return 0;
}
