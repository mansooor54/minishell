/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell_loop.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/13 15:04:45 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

static void	check_signal(t_shell *shell)
{
	if (g_signal == SIGINT)
	{
		shell->exit_status = 130;
		g_signal = 0;
	}
}

static int	handle_eof(t_shell *shell, char *line)
{
	if (!line)
	{
		if (shell->interactive)
			ft_putendl_fd("exit", 1);
		return (1);
	}
	return (0);
}

void	shell_loop(t_shell *shell)
{
	char	*line;

	while (!shell->should_exit)
	{
		setup_signals();
		check_signal(shell);
		if (shell->interactive)
			line = read_logical_line();
		else
			line = readline("");
		check_signal(shell);
		if (handle_eof(shell, line))
			break ;
		if (line && *line && !is_all_space(line))
		{
			if (shell->interactive)
				history_add_line(line, shell);
			process_line(line, shell);
		}
		free(line);
	}
}
