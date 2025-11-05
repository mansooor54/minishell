/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_pipeline_loop.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 00:00:00 by your_login        #+#    #+#             */
/*   Updated: 2025/11/05 14:39:26 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** init_pipeline - Initialize pipeline execution
**
** Allocates memory for pid array.
**
** @param cmd_count: Number of commands
** @param pids: Pointer to store allocated pid array
**
** Return: 0 on success, -1 on error
*/
int	init_pipeline(int cmd_count, pid_t **pids)
{
	*pids = malloc(sizeof(pid_t) * cmd_count);
	if (!*pids)
	{
		print_error("malloc", "failed to allocate pid array");
		return (-1);
	}
	return (0);
}

/*
** create_pipe_if_needed - Create pipe if not last command
**
** @param current: Current command
** @param pipefd: Array to store pipe fds
**
** Return: 0 on success, -1 on error
*/
int	create_pipe_if_needed(t_cmd *current, int pipefd[2])
{
	if (current->next)
	{
		if (pipe(pipefd) == -1)
		{
			print_error("pipe", strerror(errno));
			return (-1);
		}
	}
	return (0);
}

/*
** update_prev_fd - Update previous read fd for next iteration
**
** Closes old prev_read_fd and updates it with new pipe read end.
**
** @param prev_read_fd: Current previous read fd
** @param pipefd: Current pipe fds
** @param has_next: 1 if there's a next command
**
** Return: New prev_read_fd value
*/
int	update_prev_fd(int prev_read_fd, int pipefd[2], int has_next)
{
	safe_close(prev_read_fd);
	if (has_next)
	{
		safe_close(pipefd[1]);
		return (pipefd[0]);
	}
	return (-1);
}

/*
** execute_one_command - Execute one command in the pipeline
**
** Creates pipe if needed, forks child, and updates file descriptors.
**
** @param cmd: Current command
** @param shell: Shell state
** @param pids: Array to store child pid
** @param index: Index in pids array
** @param prev_read_fd: Pointer to previous read fd
**
** Return: 0 on success, -1 on error
*/
/* was: (t_cmd*, t_shell*, pid_t*, int, int*) */
int	execute_one_command(t_cmd *cmd, int index, t_pipe_ctx *ctx)
{
	int			pipefd[2];
	t_child_io	io;

	io.has_next = (cmd->next != NULL);
	if (io.has_next && pipe(pipefd) == -1)
		return (print_error("pipe", strerror(errno)), -1);
	io.pipe_rd = io.has_next ? pipefd[0] : -1;
	io.pipe_wr = io.has_next ? pipefd[1] : -1;
	io.prev_rd = *ctx->prev_rd;
	ctx->pids[index] = create_child_process(cmd, ctx->shell, &io);
	if (ctx->pids[index] == -1)
	{
		if (io.has_next)
		{
			safe_close(pipefd[0]);
			safe_close(pipefd[1]);
		}
		return (-1);
	}
	safe_close(*ctx->prev_rd);
	if (io.has_next)
	{
		safe_close(pipefd[1]);
		*ctx->prev_rd = pipefd[0];
	}
	else
		*ctx->prev_rd = -1;
	return (0);
}