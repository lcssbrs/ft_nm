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

int main(int ac, char **ag)
{
    char *filename;
	//int bit;
    int fd;
    struct stat buf;

    filename = "a.out";
    //bit = 0;
    if (ac >= 2)
        filename = ag[1];
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
    if (buf.st_size == 0)
    {
        ft_putstr_fd("Error: file is empty\n", 2);
        return (1);
    }
    void *ptr = mmap(0, buf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

    if (ptr == MAP_FAILED)
    {
        ft_putstr_fd("Error: mmap failed\n", 2);
        return (1);
    }
    ft_printf("%p\n", ptr);
    if (close(fd) < 0)
    {
        ft_putstr_fd("Error: close failed\n", 2);
        return (1);
    }

}
