/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_child_fds.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 00:00:00 by your_login        #+#    #+#             */
/*   Updated: 2025/11/05 13:11:27 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

static int	setup_input_fd(int prev_read_fd)
{
	if (prev_read_fd >= 0)
	{
		if (dup2(prev_read_fd, STDIN_FILENO) == -1)
		{
			print_error("dup2", "failed to redirect stdin");
			return (-1);
		}
		safe_close(prev_read_fd);
	}
	return (0);
}

/*
** setup_output_fd - Redirect stdout to next pipe if needed
**
** @param pipefd: Current pipe file descriptors
** @param has_next: 1 if there's a next command, 0 if last
**
** Return: 0 on success, -1 on error
*/
static int	setup_output_fd(int pipefd[2], int has_next)
{
	if (has_next)
	{
		safe_close(pipefd[0]);
		if (dup2(pipefd[1], STDOUT_FILENO) == -1)
		{
			print_error("dup2", "failed to redirect stdout");
			return (-1);
		}
		safe_close(pipefd[1]);
	}
	return (0);
}

/*
** setup_child_fds - Set up all file descriptors for child process
**
** @param pipefd: Current pipe fds
** @param prev_read_fd: Previous pipe read end
** @param has_next: 1 if not last command
**
** Return: 0 on success, -1 on error
*/
int	setup_child_fds(int pipefd[2], int prev_read_fd, int has_next)
{
	if (setup_input_fd(prev_read_fd) == -1)
		return (-1);
	if (setup_output_fd(pipefd, has_next) == -1)
		return (-1);
	return (0);
}
