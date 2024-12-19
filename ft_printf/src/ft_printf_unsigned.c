/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf_unsigned.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lseiberr <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/22 18:17:45 by lseiberr          #+#    #+#             */
/*   Updated: 2023/04/25 15:53:53 by lseiberr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../ft_printf.h"

int	ft_numberlen(unsigned int num)
{
	int	len;

	len = 0;
	while (num != 0)
	{
		num = num / 10;
		len ++;
	}
	return (len);
}

char	*ft_unsigneditoa(unsigned int n)
{
	char	*num;
	int		len;

	len = ft_numberlen(n);
	num = (char *)(malloc(sizeof(char) *(len + 1)));
	if (!num)
		return (0);
	num[len] = '\0';
	while (n != 0)
	{
		num[len - 1] = n % 10 + 48;
		n = n / 10;
		len --;
	}
	return (num);
}

int	ft_printunsigned(int n)
{
	char	*num;
	int		len;

	len = 0;
	if (n == 0)
		len += write (1, "0", 1);
	else
	{
		num = ft_unsigneditoa(n);
		len = ft_printstr(num);
		free (num);
	}
	return (len);
}
