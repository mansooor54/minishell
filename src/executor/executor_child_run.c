/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_child_run.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 00:00:00 by your_login        #+#    #+#             */
/*   Updated: 2025/11/05 15:16:08 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** execute_builtin_child - Execute builtin command in child process
**
** @param cmd: Command structure
** @param shell: Shell state
*/
static void	execute_builtin_child(t_cmd *cmd, t_shell *shell)
{
	int	exit_code;

	exit_code = execute_builtin(cmd, shell);
	exit(exit_code);
}

/*
** execute_external_child - Execute external command in child process
**
** @param cmd: Command structure
** @param shell: Shell state
*/
static void	execute_external_child(t_cmd *cmd, t_shell *shell, char *path)
{
	char	**envp;

	envp = env_to_array(shell->env);
	if (!envp)
	{
		print_error("env_to_array", "allocation failed");
		free(path);
		exit(1);
	}
	execve(path, cmd->args, envp);
	free_array(envp);
	free(path);
	print_error("execve", strerror(errno));
	exit(126);
}

/*
** execute_cmd_child - Execute command in child process
**
** This function handles the complete child execution including
** redirections and command type detection.
**
** @param cmd: Command to execute
** @param shell: Shell state
*/
static void	execute_cmd_child(t_cmd *cmd, t_shell *shell)
{
	char	*path;

	if (!cmd || !cmd->args || !cmd->args[0])
		exit(0);
	if (setup_redirections(cmd->redirs) == -1)
		exit(1);
	if (is_builtin(cmd->args[0]))
		execute_builtin_child(cmd, shell);
	else
	{
		path = find_executable(cmd->args[0], shell->env);
		if (!path)
		{
			ft_putstr_fd("minishell: ", STDERR_FILENO);
			ft_putstr_fd(cmd->args[0], STDERR_FILENO);
			ft_putendl_fd(": command not found", STDERR_FILENO);
			exit(127);
		}
		execute_external_child(cmd, shell, path);
	}
}

/*
** create_child_process - Fork and execute command in child
**
** @param cmd: Command to execute
** @param shell: Shell state
** @param pipefd: Current pipe fds
** @param prev_read_fd: Previous pipe read end
** @param has_next: 1 if not last command
**
** Return: Child PID on success, -1 on error
*/
/* was: (t_cmd*, t_shell*, int pipefd[2], int prev_read_fd, int has_next) */
pid_t	create_child_process(t_cmd *cmd, t_shell *shell, t_child_io *io)
{
	pid_t	pid;
	int		pipefd[2];

	pid = fork();
	if (pid == -1)
		return (print_error("fork", strerror(errno)), -1);
	if (pid == 0)
	{
		pipefd[0] = io->pipe_rd;
		pipefd[1] = io->pipe_wr;
		if (setup_child_fds(pipefd, io->prev_rd, io->has_next) == -1)
			exit(1);
		execute_cmd_child(cmd, shell);
		exit(1);
	}
	return (pid);
}
