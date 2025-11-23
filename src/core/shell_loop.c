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

#include "../../minishell.h"

static int	handle_eof_and_sigint(t_shell *shell, char *line)
{
	if (!line && g_shell.sigint_during_read)
	{
		g_shell.sigint_during_read = 0;
		return (1);
	}
	if (!line)
	{
		if (shell->interactive)
			ft_putendl_fd("exit", 1);
		return (2);
	}
	return (0);
}

void	shell_loop(t_shell *shell)
{
	char	*line;
	int		status;

	while (!shell->should_exit)
	{
		if (!g_shell.in_heredoc)
			setup_signals();
		if (shell->interactive)
			line = read_logical_line();
		else
			line = readline("");
		status = handle_eof_and_sigint(shell, line);
		if (status == 1)
			continue ;
		if (status == 2)
			break ;
		if (*line && !is_all_space(line))
		{
			if (shell->interactive)
				history_add_line(line);
			process_line(line, shell);
		}
		free(line);
	}
}
