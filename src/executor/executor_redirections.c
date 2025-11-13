/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_redirections.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/13 12:50:35 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	handle_heredoc(char *delimiter)
{
	int		pipe_fd[2];
	char	*line;
	char	*expanded;

	if (pipe(pipe_fd) == -1)
		return (-1);
	while (1)
	{
		line = readline("> ");
		if (!line || ft_strcmp(line, delimiter) == 0)
		{
			free(line);
			break ;
		}
		/* expand variables in heredoc line, like bash does for unquoted EOF */
		expanded = expand_variables(line, g_shell.env, g_shell.exit_status);
		if (!expanded)
		{
			free(line);
			close(pipe_fd[1]);
			close(pipe_fd[0]);
			return (-1);
		}
		write(pipe_fd[1], expanded, ft_strlen(expanded));
		write(pipe_fd[1], "\n", 1);
		free(line);
		free(expanded);
	}
	close(pipe_fd[1]);
	dup2(pipe_fd[0], STDIN_FILENO);
	close(pipe_fd[0]);
	return (0);
}

static int	handle_input(char *file)
{
	int	fd;

	fd = open(file, O_RDONLY);
	if (fd == -1)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(file, 2);
		ft_putendl_fd(": No such file or directory", 2);
		return (-1);
	}
	dup2(fd, STDIN_FILENO);
	close(fd);
	return (0);
}

static int	handle_output(char *file, int append)
{
	int	fd;
	int	flags;

	if (append)
		flags = O_WRONLY | O_CREAT | O_APPEND;
	else
		flags = O_WRONLY | O_CREAT | O_TRUNC;
	fd = open(file, flags, 0644);
	if (fd == -1)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(file, 2);
		ft_putendl_fd(": Permission denied", 2);
		return (-1);
	}
	dup2(fd, STDOUT_FILENO);
	close(fd);
	return (0);
}

int	setup_redirections(t_redir *redirs)
{
	while (redirs)
	{
		if (redirs->type == TOKEN_REDIR_IN)
		{
			if (handle_input(redirs->file) == -1)
				return (-1);
		}
		else if (redirs->type == TOKEN_REDIR_OUT)
		{
			if (handle_output(redirs->file, 0) == -1)
				return (-1);
		}
		else if (redirs->type == TOKEN_REDIR_APPEND)
		{
			if (handle_output(redirs->file, 1) == -1)
				return (-1);
		}
		else if (redirs->type == TOKEN_REDIR_HEREDOC)
		{
			if (handle_heredoc(redirs->file) == -1)
				return (-1);
		}
		redirs = redirs->next;
	}
	return (0);
}
