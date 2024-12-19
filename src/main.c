#include "../header/ft_nm.h"

char *str = "Hello, World!\n";


int main(int ac, char **ag)
{
    if (ac < 2)
    {
        print_adress((uintptr_t)ft_putstr);
        printf("\n");
        printf("%p\n", ft_putstr);
        ft_putstr("Usage: ft_nm [file]\n");
        return (1);
    }
}