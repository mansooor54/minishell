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
** exec_pipe_cmd - Execute a single command within a pipeline
**
** This function is executed in a child process and handles the plumbing
** of pipes for a command in a pipeline. It manages three types of I/O:
**
** 1. Input from previous command:
**    - If prev_fd[0] is valid, redirect it to stdin
**    - This receives output from the previous command in the pipeline
**
** 2. Output to next command:
**    - If pipe_fd[1] is valid, redirect it to stdout
**    - This sends output to the next command in the pipeline
**
** 3. File redirections:
**    - Applied after pipe setup, so they can override pipe I/O
**    - Example: "cmd1 | cmd2 > file" redirects cmd2's output to file, not pipe
**
** After setting up I/O, it executes the command (builtin or external).
** This function never returns; it always exits the child process.
**
** @param cmd: Command to execute
** @param prev_fd: File descriptors from previous pipe [read_end, write_end]
**                 prev_fd[0] = -1 if this is the first command
** @param pipe_fd: File descriptors for next pipe [read_end, write_end]
**                 pipe_fd[1] = -1 if this is the last command
** @param shell: Shell state for environment and builtins
**
** Return: Never returns (always calls exit())
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
** execute_pipeline - Execute a pipeline of commands connected by pipes
**
** This function implements the core pipeline execution logic. It handles
** the creation of pipes and coordination of multiple processes.
**
** Pipeline execution process:
** 1. Count commands - if only one, skip pipe logic and use execute_command()
** 2. For each command in the pipeline:
**    a. Create a pipe (except for the last command)
**    b. Fork a child process
**    c. In child: set up pipes and execute command
**    d. In parent: close used pipe ends and save read end for next command
** 3. Wait for all child processes to complete
** 4. Capture exit status of the last command
**
** Pipe management:
** - Each command (except last) creates a pipe for its output
** - The read end of each pipe becomes the input for the next command
** - Parent closes pipe ends after forking to prevent descriptor leaks
** - prev_fd tracks the read end from the previous iteration
**
** Example: "ls | grep txt | wc -l"
** - ls writes to pipe1, grep reads from pipe1
** - grep writes to pipe2, wc reads from pipe2
** - wc writes to stdout (no pipe)
**
** @param pipeline: Pipeline structure containing linked list of commands
** @param shell: Shell state structure
**
** Return: void (updates shell->exit_status with last command's exit status)
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
** executor - Main executor function with logical operator support
**
** This is the top-level executor that handles pipelines connected by
** logical operators (&& and ||). It implements short-circuit evaluation:
**
** Logical AND (&&):
** - Execute first pipeline
** - If exit status is non-zero (failure), stop execution
** - If exit status is zero (success), continue to next pipeline
** - Example: "make && ./program" only runs program if make succeeds
**
** Logical OR (||):
** - Execute first pipeline
** - If exit status is zero (success), stop execution
** - If exit status is non-zero (failure), continue to next pipeline
** - Example: "test -f file || touch file" creates file only if it doesn't exist
**
** This function iterates through the pipeline list, executing each one
** and checking the logical operator to determine whether to continue.
** The exit status is preserved across the entire chain.
**
** Note: This is a BONUS feature. The mandatory part only requires single
** pipelines without logical operators.
**
** @param pipeline: Linked list of pipelines connected by logical operators
**                  Each pipeline has a logic_op field (TOKEN_AND, TOKEN_OR, or TOKEN_EOF)
** @param shell: Shell state structure
**
** Return: void (updates shell->exit_status)
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
