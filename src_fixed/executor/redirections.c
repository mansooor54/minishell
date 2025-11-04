/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/04 16:00:00 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** handle_heredoc - Handle heredoc input redirection (<<)
**
** @param delimiter: String that marks the end of heredoc input
** Return: 0 on success, -1 if pipe creation fails
*/
static int	handle_heredoc(char *delimiter)
{
	int		pipe_fd[2];
	char	*line;

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
		write(pipe_fd[1], line, ft_strlen(line));
		write(pipe_fd[1], "\n", 1);
		free(line);
	}
	close(pipe_fd[1]);
	dup2(pipe_fd[0], STDIN_FILENO);
	close(pipe_fd[0]);
	return (0);
}

/*
** handle_input - Handle input redirection from file (<)
**
** @param file: Path to the file to read from
** Return: 0 on success, -1 if file cannot be opened
*/
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

/*
** handle_output - Handle output redirection to file (> or >>)
**
** @param file: Path to the output file
** @param append: 1 for append mode (>>), 0 for truncate mode (>)
** Return: 0 on success, -1 if file cannot be opened/created
*/
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

/*
** process_single_redir - Process a single redirection
**
** @param redir: Redirection structure
** Return: 0 on success, -1 on failure
*/
static int	process_single_redir(t_redir *redir)
{
	if (redir->type == TOKEN_REDIR_IN)
		return (handle_input(redir->file));
	else if (redir->type == TOKEN_REDIR_OUT)
		return (handle_output(redir->file, 0));
	else if (redir->type == TOKEN_REDIR_APPEND)
		return (handle_output(redir->file, 1));
	else if (redir->type == TOKEN_REDIR_HEREDOC)
		return (handle_heredoc(redir->file));
	return (0);
}

/*
** setup_redirections - Set up all redirections for a command
**
** @param redirs: Linked list of redirection structures
** Return: 0 if all redirections succeed, -1 if any fails
*/
int	setup_redirections(t_redir *redirs)
{
	while (redirs)
	{
		if (process_single_redir(redirs) == -1)
			return (-1);
		redirs = redirs->next;
	}
	return (0);
}
