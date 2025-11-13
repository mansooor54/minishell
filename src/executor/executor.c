/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 00:00:00 by your_login        #+#    #+#             */
/*   Updated: 2025/11/05 13:11:27 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*resolve_command_path(char *cmd, t_shell *shell)
{
	char	*path;

	if (has_slash(cmd))
		path = ft_strdup(cmd);
	else
		path = find_executable(cmd, shell->env);
	return (path);
}

static inline void	put2(const char *s)
{
	if (s)
		write(2, s, ft_strlen(s));
}

static int	handle_path_errors(char *cmd, char *path, t_shell *shell)
{
	if (!path)
	{
		cmd_not_found(cmd);
		shell->exit_status = 127;
		return (1);
	}
	if (is_directory(path))
	{
		put2("minishell: ");
		put2(cmd);
		put2(": is a directory\n");
		free(path);
		shell->exit_status = 126;
		return (1);
	}
	return (0);
}

static int	check_execution_permission(char *cmd, char *path, t_shell *shell)
{
	if (access(path, X_OK) == -1)
	{
		if (errno == EACCES)
		{
			perror_with_cmd(cmd);
			shell->exit_status = 126;
		}
		else
		{
			perror_with_cmd(cmd);
			shell->exit_status = 127;
		}
		free(path);
		return (1);
	}
	return (0);
}

static void	execute_with_path(char *cmd, char *path, char **argv, char **envp)
{	
	execve(path, argv, envp);
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(cmd, 2);
	ft_putstr_fd(": ", 2);
	ft_putendl_fd(strerror(errno), 2);
	if (errno == EACCES)
		exit(126);
	else
		exit(127);
}

void	execute_command(char **argv, t_shell *shell, char **envp)
{
	char	*path;

	if (!argv || !argv[0] || !argv[0][0])
	{
		shell->exit_status = 0;
		return ;
	}
	path = resolve_command_path(argv[0], shell);
	if (handle_path_errors(argv[0], path, shell))
		return ;
	if (check_execution_permission(argv[0], path, shell))
		return ;
	execute_with_path(argv[0], path, argv, envp);
	perror_with_cmd(argv[0]);
	free(path);
	if (errno == EACCES)
		shell->exit_status = 126;
	else
		shell->exit_status = 127;
}
