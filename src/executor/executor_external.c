/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_external.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 00:00:00 by your_login        #+#    #+#             */
/*   Updated: 2025/11/05 13:11:27 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

static int	handle_directory_check(char *cmd, t_shell *shell)
{
	struct stat	st;

	if (stat(cmd, &st) == 0 && S_ISDIR(st.st_mode))
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(cmd, 2);
		ft_putendl_fd(": is a directory", 2);
		shell->exit_status = 126;
		return (1);
	}
	return (0);
}

static int	handle_path_resolution(t_cmd *cmd, t_shell *shell, char **path)
{
	if (ft_strchr(cmd->args[0], '/'))
	{
		if (handle_directory_check(cmd->args[0], shell))
			return (1);
		if (access(cmd->args[0], F_OK) != 0)
		{
			ft_putstr_fd("minishell: ", 2);
			ft_putstr_fd(cmd->args[0], 2);
			ft_putstr_fd(": ", 2);
			ft_putendl_fd(strerror(errno), 2);
			shell->exit_status = 127;
			return (1);
		}
		*path = ft_strdup(cmd->args[0]);
	}
	else
	{
		*path = find_executable(cmd->args[0], shell->env);
	}
	if (!*path)
	{
		cmd_not_found(cmd->args[0]);
		shell->exit_status = 127;
		return (1);
	}
	return (0);
}

static void	execute_child_process(t_cmd *cmd, t_shell *shell, char *path)
{
	if (setup_redirections(cmd->redirs) == -1)
		exit(1);
	execve(path, cmd->args, env_to_array(shell->env));
	if (errno == EACCES)
		exit(126);
	else
		exit(127);
}

static void	handle_parent_process(pid_t pid, t_shell *shell)
{
	int	status;

	status = 0;
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	if (waitpid(pid, &status, 0) == -1)
	{
		print_error("waitpid", strerror(errno));
		shell->exit_status = 1;
	}
	else
	{
		if (WIFEXITED(status))
			shell->exit_status = WEXITSTATUS(status);
		else if (WIFSIGNALED(status))
			shell->exit_status = 128 + WTERMSIG(status);
	}
	signal(SIGINT, handle_sigint);
	signal(SIGQUIT, handle_sigquit);
}

void	execute_external(t_cmd *cmd, t_shell *shell)
{
	char	*path;
	pid_t	pid;

	if (!cmd || !cmd->args || !cmd->args[0])
		return ;
	if (handle_path_resolution(cmd, shell, &path))
		return ;
	pid = fork();
	if (pid == -1)
	{
		print_error("fork", strerror(errno));
		free(path);
		shell->exit_status = 127;
		return ;
	}
	if (pid == 0)
		execute_child_process(cmd, shell, path);
	free(path);
	handle_parent_process(pid, shell);
}
