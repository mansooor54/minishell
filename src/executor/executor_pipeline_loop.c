/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_pipeline_loop.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 00:00:00 by your_login        #+#    #+#             */
/*   Updated: 2025/11/05 15:46:41 by malmarzo         ###   ########.fr       */
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

/* prepare pipe + child io */
static int	prepare_child_io(t_cmd *cmd, int prev_rd,
				int pipefd[2], t_child_io *io)
{
	int	has_next;

	has_next = 0;
	if (cmd && cmd->next)
		has_next = 1;
	io->has_next = has_next;
	io->prev_rd = prev_rd;
	if (has_next)
	{
		if (pipe(pipefd) == -1)
		{
			print_error("pipe", strerror(errno));
			return (-1);
		}
		io->pipe_rd = pipefd[0];
		io->pipe_wr = pipefd[1];
	}
	else
	{
		io->pipe_rd = -1;
		io->pipe_wr = -1;
	}
	return (0);
}

/* close fds and move read end to prev_rd */
static void	finalize_after_fork(t_child_io *io, int pipefd[2], int *prev_rd)
{
	safe_close(*prev_rd);
	if (io->has_next)
	{
		safe_close(pipefd[1]);
		*prev_rd = pipefd[0];
	}
	else
		*prev_rd = -1;
}

int	execute_one_command(t_cmd *cmd, int index, t_pipe_ctx *ctx)
{
	int			pipefd[2];
	t_child_io	io;

	if (prepare_child_io(cmd, *ctx->prev_rd, pipefd, &io) == -1)
		return (-1);
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
	finalize_after_fork(&io, pipefd, ctx->prev_rd);
	return (0);
}
