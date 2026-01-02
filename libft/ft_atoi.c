/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/16 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/01/16 09:15:37 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static long	handle_overflow(long result, long digit, int sign)
{
	if (result > LONG_MAX / 10)
	{
		if (sign == 1)
			return (LONG_MAX);
		else
			return (LONG_MIN);
	}
	result = result * 10;
	if (result > LONG_MAX - digit)
	{
		if (sign == 1)
			return (LONG_MAX);
		else
			return (LONG_MIN);
	}
	return (result + digit);
}

static void	skip_spaces_sign(const char *str, int *i, int *sign)
{
	*i = 0;
	*sign = 1;
	while (str[*i] == 32 || (str[*i] >= 9 && str[*i] <= 13))
		(*i)++;
	if (str[*i] == '-' || str[*i] == '+')
	{
		if (str[*i] == '-')
			*sign = -1;
		(*i)++;
	}
}

long	ft_atoi(const char *str)
{
	int		i;
	int		sign;
	long	result;
	long	tmp;
	long	digit;

	result = 0;
	skip_spaces_sign(str, &i, &sign);
	while (str[i] >= '0' && str[i] <= '9')
	{
		digit = str[i] - '0';
		tmp = handle_overflow(result, digit, sign);
		if ((sign == 1 && tmp == LONG_MAX)
			|| (sign == -1 && tmp == LONG_MIN))
			return (tmp);
		result = tmp;
		i++;
	}
	return (result * sign);
}
