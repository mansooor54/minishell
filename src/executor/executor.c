/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/05 15:38:35 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

static void	child_exec_external(t_cmd *cmd, t_shell *shell, char *path)
{
	if (setup_redirections(cmd->redirs) == -1)
		exit(1);
	execve(path, cmd->args, env_to_array(shell->env));
	exit(1);
}

static void	wait_and_set_status(pid_t pid, t_shell *shell)
{
	int	st;

	waitpid(pid, &st, 0);
	if (WIFEXITED(st))
		shell->exit_status = WEXITSTATUS(st);
}

static void	execute_external(t_cmd *cmd, t_shell *shell)
{
	char	*path;
	pid_t	pid;

	path = find_executable(cmd->args[0], shell->env);
	if (!path)
		return (ft_putstr_fd("minishell: ", 2),
			ft_putstr_fd(cmd->args[0], 2),
			ft_putendl_fd(": command not found", 2),
			(void)(shell->exit_status = 127));
	pid = fork();
	if (pid == 0)
		child_exec_external(cmd, shell, path);
	wait_and_set_status(pid, shell);
	free(path);
}

static void	execute_builtin_with_redir(t_cmd *cmd, t_shell *shell)
{
	pid_t	pid;
	int		status;

	pid = fork();
	if (pid == 0)
	{
		if (setup_redirections(cmd->redirs) == -1)
			exit(1);
		exit(execute_builtin(cmd, shell));
	}
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		shell->exit_status = WEXITSTATUS(status);
}

void	execute_command(t_cmd *cmd, t_shell *shell)
{
	if (!cmd || !cmd->args || !cmd->args[0])
		return ;
	if (is_builtin(cmd->args[0]))
	{
		if (cmd->redirs)
			execute_builtin_with_redir(cmd, shell);
		else
			shell->exit_status = execute_builtin(cmd, shell);
	}
	else
		execute_external(cmd, shell);
}
