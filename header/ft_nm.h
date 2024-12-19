#ifndef FT_NM_H
#define FT_NM_H

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../src/ft_putstr.c"
#include "../src/print_adress.c"

void ft_putstr(char *str);
void print_adress(uintptr_t adress);

#endif