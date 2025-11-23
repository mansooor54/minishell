/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_child_run.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 00:00:00 by your_login        #+#    #+#             */
/*   Updated: 2025/11/13 14:25:43 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

static void	execute_builtin_child(t_cmd *cmd, t_shell *shell)
{
	int	exit_code;

	exit_code = execute_builtin(cmd, shell);
	exit(exit_code);
}

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
			cmd_not_found(cmd->args[0]);
			shell->exit_status = 127;
			return ;
		}
		execute_external_child(cmd, shell, path);
	}
}

pid_t	create_child_process(t_cmd *cmd, t_shell *shell, t_child_io *io)
{
	pid_t	pid;
	int		pipefd[2];

	pid = fork();
	if (pid == -1)
	{
		print_error("fork", strerror(errno));
		return (-1);
	}
	if (pid == 0)
	{
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		pipefd[0] = io->pipe_rd;
		pipefd[1] = io->pipe_wr;
		if (setup_child_fds(pipefd, io->prev_rd, io->has_next) == -1)
			exit(1);
		execute_cmd_child(cmd, shell);
		exit(1);
	}
	return (pid);
}
