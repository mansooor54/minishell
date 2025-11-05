/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_pipeline_run.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 00:00:00 by your_login        #+#    #+#             */
/*   Updated: 2025/11/05 15:16:27 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** execute_pipeline_loop - Main loop for pipeline execution
**
** Iterates through commands, creating pipes and forking children.
**
** @param cmds: Command list
** @param shell: Shell state
** @param pids: Array to store child pids
** @param cmd_count: Number of commands
**
** Return: 0 on success, -1 on error
*/
int	execute_pipeline_loop(t_cmd *cmds, t_shell *shell,
				pid_t *pids, int cmd_count)
{
	int			i;
	int			prev_read_fd;
	t_cmd		*current;
	t_pipe_ctx	ctx;

	i = 0;
	current = cmds;
	prev_read_fd = -1;
	ctx.shell = shell;
	ctx.pids = pids;
	ctx.prev_rd = &prev_read_fd;
	while (current && i < cmd_count)
	{
		if (execute_one_command(current, i, &ctx) == -1)
			return (-1);
		current = current->next;
		i++;
	}
	return (0);
}

int	execute_pipeline(t_cmd *cmds, t_shell *shell)
{
	int		cmd_count;
	pid_t	*pids;
	int		exit_status;

	if (!cmds)
		return (0);
	cmd_count = count_commands(cmds);
	if (init_pipeline(cmd_count, &pids) == -1)
		return (1);
	if (execute_pipeline_loop(cmds, shell, pids, cmd_count) == -1)
	{
		free(pids);
		return (1);
	}
	exit_status = wait_for_children(pids, cmd_count);
	free(pids);
	return (exit_status);
}

int	wait_for_children(pid_t *pids, int count)
{
	int	i;
	int	status;
	int	last_status;

	last_status = 0;
	i = 0;
	while (i < count)
	{
		if (pids[i] > 0 && waitpid(pids[i], &status, 0) == -1)
			print_error("waitpid", strerror(errno));
		else if (pids[i] > 0 && WIFEXITED(status))
			last_status = WEXITSTATUS(status);
		else if (pids[i] > 0 && WIFSIGNALED(status))
			last_status = 128 + WTERMSIG(status);
		i++;
	}
	return (last_status);
}
