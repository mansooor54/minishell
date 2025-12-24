/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_num.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/23 12:30:00 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/23 12:30:00 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

static int	skip_whitespace_sign(char *str, int *sign)
{
	int	i;

	i = 0;
	*sign = 1;
	while (str[i] == 32 || (str[i] >= 9 && str[i] <= 13))
		i++;
	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i] == '-')
			*sign = -1;
		i++;
	}
	while (str[i] == '0')
		i++;
	return (i);
}

static int	check_digit_overflow(char *str, int i, unsigned long long limit)
{
	unsigned long long	result;

	result = 0;
	while (str[i] >= '0' && str[i] <= '9')
	{
		if (result > limit / 10)
			return (1);
		result = result * 10;
		if (result > limit - (str[i] - '0'))
			return (1);
		result = result + (str[i] - '0');
		i++;
	}
	return (0);
}

int	is_numeric_overflow(char *str)
{
	int					i;
	int					sign;
	unsigned long long	limit;

	i = skip_whitespace_sign(str, &sign);
	if (ft_strlen(&str[i]) > 19)
		return (1);
	if (sign == -1)
		limit = (unsigned long long)LONG_MAX + 1;
	else
		limit = (unsigned long long)LONG_MAX;
	return (check_digit_overflow(str, i, limit));
}

int	is_valid_number(char *str)
{
	int	i;
	int	has_digits;

	i = 0;
	has_digits = 0;
	while (str[i] == 32 || (str[i] >= 9 && str[i] <= 13))
		i++;
	if (str[i] == '-' || str[i] == '+')
		i++;
	while (str[i] >= '0' && str[i] <= '9')
	{
		has_digits = 1;
		i++;
	}
	while (str[i] == 32 || (str[i] >= 9 && str[i] <= 13))
		i++;
	return (has_digits && str[i] == '\0');
}
