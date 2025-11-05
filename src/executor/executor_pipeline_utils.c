/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_pipeline_utils.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 00:00:00 by your_login        #+#    #+#             */
/*   Updated: 2025/11/05 15:05:03 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** print_error - Print formatted error message to stderr
**
** @param function: Name of the function that failed
** @param message: Error message or strerror(errno)
*/
void	print_error(const char *function, const char *message)
{
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	ft_putstr_fd((char *)function, STDERR_FILENO);
	ft_putstr_fd(": ", STDERR_FILENO);
	ft_putstr_fd((char *)message, STDERR_FILENO);
	ft_putstr_fd("\n", STDERR_FILENO);
}

/*
** safe_close - Safely close a file descriptor
**
** Closes fd if valid (>= 0) and prints error if close fails.
**
** @param fd: File descriptor to close
*/
void	safe_close(int fd)
{
	if (fd >= 0)
	{
		if (close(fd) == -1)
			print_error("close", strerror(errno));
	}
}

/*
** cleanup_pipe - Close both ends of a pipe
**
** @param pipefd: Array containing pipe file descriptors [read, write]
*/
void	cleanup_pipe(int pipefd[2])
{
	safe_close(pipefd[0]);
	safe_close(pipefd[1]);
}

/*
** count_commands - Count the number of commands in a pipeline
**
** This helper function traverses the linked list of commands and counts
** how many commands are present. This count is used to determine:
** - Whether we need to create pipes (count > 1)
** - How many child processes to wait for
** - Optimization: single commands can skip pipe creation
**
** @param cmds: Head of the command linked list
**
** Return: Number of commands in the list (0 if cmds is NULL)
*/
int	count_commands(t_cmd *cmds)
{
	int		count;
	t_cmd	*current;

	count = 0;
	current = cmds;
	while (current)
	{
		count++;
		current = current->next;
	}
	return (count);
}
