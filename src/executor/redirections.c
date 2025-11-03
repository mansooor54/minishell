/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirections.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/03 11:55:37 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** handle_heredoc - Handle heredoc input redirection (<<)
**
** Heredoc allows multi-line input until a delimiter is encountered.
** This function implements the heredoc behavior:
**
** 1. Create a pipe to store the heredoc content
** 2. Read lines from stdin using readline() with "> " prompt
** 3. For each line:
**    - If line is NULL (Ctrl-D) or matches delimiter, stop reading
**    - Otherwise, write line to pipe (with newline)
** 4. Close write end of pipe
** 5. Redirect read end of pipe to stdin
**
** Important notes:
** - According to the subject, heredoc should NOT update history
**   (we use readline but don't call add_history)
** - The delimiter is compared exactly (no variable expansion)
** - Each line is written with a newline character
**
** Example usage:
**   cat << EOF
**   line 1
**   line 2
**   EOF
**
** @param delimiter: String that marks the end of heredoc input
**
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
** This function redirects stdin to read from a file instead of the terminal.
** It implements the '<' operator behavior:
**
** 1. Open the specified file in read-only mode
** 2. If file doesn't exist or can't be opened, print error and return -1
** 3. Use dup2() to redirect the file descriptor to stdin (fd 0)
** 4. Close the original file descriptor (no longer needed after dup2)
**
** After this function, any read from stdin will actually read from the file.
**
** Error handling:
** - If file doesn't exist: "No such file or directory"
** - If file can't be read: open() fails and returns -1
**
** Example: "cat < input.txt" reads from input.txt instead of keyboard
**
** @param file: Path to the file to read from
**
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
** This function redirects stdout to write to a file instead of the terminal.
** It handles both truncate (>) and append (>>) modes:
**
** Truncate mode (>):
** - Creates file if it doesn't exist
** - Truncates (empties) file if it exists
** - Writes from the beginning
**
** Append mode (>>):
** - Creates file if it doesn't exist
** - Preserves existing content
** - Writes at the end of file
**
** Implementation:
** 1. Set appropriate flags based on append parameter
** 2. Open file with mode 0644 (rw-r--r--)
** 3. If open fails, print error and return -1
** 4. Use dup2() to redirect stdout to the file
** 5. Close original file descriptor
**
** After this function, any write to stdout will go to the file.
**
** Examples:
** - "echo hello > file.txt" creates/overwrites file.txt
** - "echo world >> file.txt" appends to file.txt
**
** @param file: Path to the output file
** @param append: 1 for append mode (>>), 0 for truncate mode (>)
**
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
** setup_redirections - Set up all redirections for a command
**
** This function processes a linked list of redirections and applies them
** in order. The order is important because later redirections can override
** earlier ones.
**
** Supported redirection types:
** - TOKEN_REDIR_IN (<): Input from file
** - TOKEN_REDIR_OUT (>): Output to file (truncate)
** - TOKEN_REDIR_APPEND (>>): Output to file (append)
** - TOKEN_REDIR_HEREDOC (<<): Input from heredoc
**
** Processing order matters:
** Example: "cmd < in1 < in2" will use in2 (last input redirection wins)
** Example: "cmd > out1 > out2" will write to out2 (last output wins)
**
** Error handling:
** - If any redirection fails, immediately return -1
** - This causes the command execution to abort
** - Partial redirections may have been applied before failure
**
** This function is called in child processes, so any file descriptor
** modifications only affect the child, not the parent shell.
**
** @param redirs: Linked list of redirection structures
**                Each contains type (TOKEN_REDIR_*) and file path
**
** Return: 0 if all redirections succeed, -1 if any fails
*/
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
