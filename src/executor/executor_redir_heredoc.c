/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_redir_heredoc.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/13 12:50:35 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	process_heredoc_line(char *delimiter, int pipe_fd[2])
{
	char	*line;
	char	*expanded;

	while (1)
	{
		line = readline("> ");
		if (!line || ft_strcmp(line, delimiter) == 0)
		{
			free(line);
			break ;
		}
		expanded = expand_variables(line, g_shell.env, g_shell.exit_status);
		if (!expanded)
		{
			free(line);
			return (-1);
		}
		write(pipe_fd[1], expanded, ft_strlen(expanded));
		write(pipe_fd[1], "\n", 1);
		free(line);
		free(expanded);
	}
	return (0);
}

int	handle_heredoc(char *delimiter)
{
	int	pipe_fd[2];

	if (pipe(pipe_fd) == -1)
		return (-1);
	if (process_heredoc_line(delimiter, pipe_fd) == -1)
	{
		close(pipe_fd[1]);
		close(pipe_fd[0]);
		return (-1);
	}
	close(pipe_fd[1]);
	dup2(pipe_fd[0], STDIN_FILENO);
	close(pipe_fd[0]);
	return (0);
}
