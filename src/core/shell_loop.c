/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/13 15:04:45 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	handle_empty_line(char *line, t_shell *shell)
{
	if (!line)
	{
		if (shell->exit_status == 130)
			return (1);
		ft_putendl_fd("exit", 1);
		return (0);
	}
	return (2);
}

void	shell_loop(t_shell *shell)
{
	char	*line;
	int		status;

	while (!shell->should_exit)
	{
		line = read_logical_line();
		status = handle_empty_line(line, shell);
		if (status == 0)
			break ;
		if (status == 1)
			continue ;
		if (*line && !is_all_space(line))
		{
			history_add_line(line);
			process_line(line, shell);
		}
		free(line);
	}
	history_save(shell);
}
