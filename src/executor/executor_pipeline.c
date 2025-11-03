/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_pipeline.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/03 11:55:37 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Count number of commands in pipeline
** Used to determine how many pipes are needed
*/
static int	count_commands(t_cmd *cmds)
{
	int	count;

	count = 0;
	while (cmds)
	{
		count++;
		cmds = cmds->next;
	}
	return (count);
}

/*
** Execute a command in a pipeline with proper pipe setup
** Handles input from previous pipe and output to next pipe
*/
static void	exec_pipe_cmd(t_cmd *cmd, int *prev_fd, int *pipe_fd,
		t_shell *shell)
{
	char	*path;
	char	**envp;

	if (prev_fd[0] != -1)
	{
		dup2(prev_fd[0], STDIN_FILENO);
		close(prev_fd[0]);
	}
	if (pipe_fd[1] != -1)
	{
		close(pipe_fd[0]);
		dup2(pipe_fd[1], STDOUT_FILENO);
		close(pipe_fd[1]);
	}
	if (setup_redirections(cmd->redirs) == -1)
		exit(1);
	if (is_builtin(cmd->args[0]))
		exit(execute_builtin(cmd, shell));
	path = find_executable(cmd->args[0], shell->env);
	if (!path)
		exit(127);
	envp = env_to_array(shell->env);
	execve(path, cmd->args, envp);
	exit(1);
}

/*
** Execute a pipeline of commands connected by pipes
** Creates pipes between commands and manages file descriptors
*/
void	execute_pipeline(t_pipeline *pipeline, t_shell *shell)
{
	t_cmd	*cmd;
	int		pipe_fd[2];
	int		prev_fd[2];
	pid_t	pid;
	int		status;
	int		cmd_count;
	int		i;

	cmd = pipeline->cmds;
	cmd_count = count_commands(cmd);
	if (cmd_count == 1)
	{
		execute_command(cmd, shell);
		return ;
	}
	prev_fd[0] = -1;
	prev_fd[1] = -1;
	i = 0;
	while (cmd)
	{
		if (cmd->next)
			pipe(pipe_fd);
		else
		{
			pipe_fd[0] = -1;
			pipe_fd[1] = -1;
		}
		pid = fork();
		if (pid == 0)
			exec_pipe_cmd(cmd, prev_fd, pipe_fd, shell);
		if (prev_fd[0] != -1)
			close(prev_fd[0]);
		if (pipe_fd[1] != -1)
			close(pipe_fd[1]);
		prev_fd[0] = pipe_fd[0];
		cmd = cmd->next;
		i++;
	}
	while (i-- > 0)
		wait(&status);
	if (WIFEXITED(status))
		shell->exit_status = WEXITSTATUS(status);
}

/*
** Main executor function
** Handles logical operators (&& and ||) for bonus part
*/
void	executor(t_pipeline *pipeline, t_shell *shell)
{
	while (pipeline)
	{
		execute_pipeline(pipeline, shell);
		if (pipeline->logic_op == TOKEN_AND && shell->exit_status != 0)
			break ;
		if (pipeline->logic_op == TOKEN_OR && shell->exit_status == 0)
			break ;
		pipeline = pipeline->next;
	}
}
