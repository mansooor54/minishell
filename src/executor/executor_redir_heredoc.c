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

#include "../../include/minishell.h"

static int	write_heredoc_line(int fd, char *ln, int qt, t_shell *sh)
{
	char	*exp;

	exp = get_expanded_line(ln, qt, sh->env, sh->exit_status);
	if (!exp)
		return (-1);
	write(fd, exp, ft_strlen(exp));
	write(fd, "\n", 1);
	free(exp);
	return (0);
}

static int	process_heredoc_line(int fd, char *cln, int qt, t_shell *sh)
{
	char	*line;

	line = readline("> ");
	if (g_signal == SIGINT)
	{
		free(line);
		return (1);
	}
	if (check_heredoc_end(line, cln))
	{
		free(line);
		return (1);
	}
	if (write_heredoc_line(fd, line, qt, sh) == -1)
	{
		free(line);
		return (-1);
	}
	free(line);
	return (0);
}

static int	read_heredoc_lines(int fd, char *cln, int qt, t_shell *sh)
{
	int	result;

	while (1)
	{
		if (g_signal == SIGINT)
			break ;
		result = process_heredoc_line(fd, cln, qt, sh);
		if (result != 0)
			return (result);
	}
	return (0);
}

static int	finalize_heredoc(int *pipe_fd)
{
	int	ret;

	close(pipe_fd[1]);
	setup_signals();
	if (g_signal == SIGINT)
	{
		close(pipe_fd[0]);
		g_signal = 0;
		return (-1);
	}
	ret = dup2(pipe_fd[0], STDIN_FILENO);
	close(pipe_fd[0]);
	if (ret == -1)
		return (-1);
	return (0);
}

int	handle_heredoc(char *delimiter, t_shell *shell)
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
	g_signal = 0;
	setup_signals();
	if (read_heredoc_lines(pipe_fd[1], clean, quoted, shell) == -1)
	{
		free(clean);
		cleanup_pipe(pipe_fd);
		return (-1);
	}
	free(clean);
	return (finalize_heredoc(pipe_fd));
}
