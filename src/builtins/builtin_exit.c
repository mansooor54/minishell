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

/*
** Check if string is a valid number
** Returns 1 if numeric, 0 otherwise
*/

static void	exit_numeric_error(char *arg)
{
	ft_putstr_fd("minishell: exit: ", 2);
	ft_putstr_fd(arg, 2);
	ft_putstr_fd(": numeric argument required\n", 2);
	exit(255);
}

/*
** Exit builtin command
** Exits the shell with optional exit code
** Returns exit code or 0
*/
int	builtin_exit(char **args, t_shell *shell)
{
	long	exit_code;
	char	*arg;

	if (args[1] && args[2])
	{
		ft_putstr_fd("minishell: exit: too many arguments\n", 2);
		shell->exit_status = 1;
		return (1);
	}
	if (!args[1])
		exit_code = shell->exit_status;
	else
	{
		arg = args[1];
		if (!is_valid_number(arg) || is_numeric_overflow(arg))
			exit_numeric_error(arg);
		exit_code = ft_atoi(arg);
		exit_code = exit_code % 256;
		if (exit_code < 0)
			exit_code += 256;
	}
	exit(exit_code);
}
