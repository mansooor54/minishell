/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_redir_heredoc.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/23 11:48:00 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

static int	write_heredoc_line(int pipe_fd, char *line, int quoted)
{
	char	*exp;

	exp = get_expanded_line(line, quoted);
	if (!exp)
		return (-1);
	write(pipe_fd, exp, ft_strlen(exp));
	write(pipe_fd, "\n", 1);
	free(exp);
	return (0);
}

static int	process_heredoc_line(int pipe_fd, char *clean, int quoted)
{
	char	*line;

	line = readline("> ");
	if (g_shell.heredoc_sigint)
	{
		free(line);
		return (1);
	}
	if (check_heredoc_end(line, clean))
	{
		free(line);
		return (1);
	}
	if (write_heredoc_line(pipe_fd, line, quoted) == -1)
	{
		free(line);
		return (-1);
	}
	free(line);
	return (0);
}

static int	read_heredoc_lines(int pipe_fd, char *clean, int quoted)
{
	int	result;

	while (1)
	{
		if (g_shell.heredoc_sigint)
			break ;
		result = process_heredoc_line(pipe_fd, clean, quoted);
		if (result != 0)
			return (result);
	}
	return (0);
}

static int	finalize_heredoc(int *pipe_fd)
{
	close(pipe_fd[1]);
	g_shell.in_heredoc = 0;
	setup_signals();
	if (g_shell.heredoc_sigint)
	{
		close(pipe_fd[0]);
		g_shell.exit_status = 130;
		return (-1);
	}
	dup2(pipe_fd[0], STDIN_FILENO);
	close(pipe_fd[0]);
	return (0);
}

int	handle_heredoc(char *delimiter)
{
	int		pipe_fd[2];
	int		quoted;
	char	*clean;

	if (pipe(pipe_fd) == -1)
		return (-1);
	clean = clean_delimiter(delimiter, &quoted);
	if (!clean)
	{
		cleanup_pipe(pipe_fd);
		return (-1);
	}
	g_shell.in_heredoc = 1;
	g_shell.heredoc_sigint = 0;
	setup_signals();
	if (read_heredoc_lines(pipe_fd[1], clean, quoted) == -1)
	{
		free(clean);
		cleanup_pipe(pipe_fd);
		return (-1);
	}
	free(clean);
	return (finalize_heredoc(pipe_fd));
}
