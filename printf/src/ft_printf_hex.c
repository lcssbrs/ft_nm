#include <unistd.h>

int	ft_hex_len(long long num)
{
	int len;

	len = 0;
	if (num == 0)
		return (1);  // Si num == 0, la longueur est 1 pour afficher '0'
	while (num != 0)
	{
		len++;
		num = num / 16;
	}
	return (len);
}

void	ft_puthex(long long num)
{
	char	*base;

	base = "0123456789abcdef";  // Utilisation de l'alphabet hexadécimal en minuscules
	if (num >= 16)
	{
		ft_puthex(num / 16);
		ft_puthex(num % 16);
	}
	else
	{
		write(1, &base[num], 1);
	}
}

int	ft_printhex(long long num)
{
	int len;
	int width = 16;  // Largeur totale à atteindre (16 caractères)
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
	ft_puthex(num);

	// Retourner la longueur totale : "0x" + adresse en hex (avec padding)
	return (len + 2 + (width - 2 - len));  // +2 pour "0x" et + padding pour les zéros
}

