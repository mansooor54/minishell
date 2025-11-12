/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exit.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/12 11:09:40 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

static int	parse_sign(const char *s, int *i)
{
	int	sign;

	sign = 1;
	if (s[*i] == '+' || s[*i] == '-')
	{
		if (s[*i] == '-')
			sign = -1;
		(*i)++;
	}
	return (sign);
}

static int	parse_digits(const char *s, int *i, long long *val)
{
	long long	v;
	int			j;

	j = *i;
	if (!ft_isdigit((unsigned char)s[j]))
		return (0);
	v = 0;
	while (ft_isdigit((unsigned char)s[j]))
	{
		v = v * 10 + (s[j] - '0');
		j++;
	}
	*i = j;
	*val = v;
	return (1);
}

/*
** Check if string is a valid number
** Returns 1 if numeric, 0 otherwise
*/
static int	is_numeric_ll(const char *s, long long *out)
{
	long long	val;
	int			sign;
	int			i;

	if (!s || !*s)
		return (0);
	i = 0;
	sign = parse_sign(s, &i);
	if (!parse_digits(s, &i, &val))
		return (0);
	if (s[i] != '\0')
		return (0);
	*out = val * sign;
	return (1);
}

/*
** Exit builtin command
** Exits the shell with optional exit code
** Returns exit code or 0
*/
int	builtin_exit(char **args, t_shell *shell)
{
	long long	ll;
	int			exit_code;

	ft_putendl_fd("exit", 1);
	if (args[1])
	{
		if (!is_numeric_ll(args[1], &ll))
		{
			ft_putstr_fd("minishell: exit: ", 2);
			ft_putstr_fd(args[1], 2);
			ft_putendl_fd(": numeric argument required", 2);
			shell->should_exit = 1;
			return (255);
		}
		if (args[2])
		{
			ft_putendl_fd("minishell: exit: too many arguments", 2);
			return (1);
		}
		exit_code = (unsigned char)ll;
		shell->should_exit = 1;
		return (exit_code);
	}
	shell->should_exit = 1;
	return (0);
}
