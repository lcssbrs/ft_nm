#include "../header/ft_nm.h"

void print_adress(uintptr_t adress)
{
    int i;
    char *hex = "0123456789abcdef";
    char str[9];

    i = 0;
    while (i < 16)
    {
        str[i] = hex[(adress >> (4 * (15 - i))) & 0xf];
        i++;
    }
    str[i] = '\0';
    ft_putstr(str);
}