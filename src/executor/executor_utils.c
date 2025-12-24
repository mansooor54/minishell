/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 00:00:00 by your_login        #+#    #+#             */
/*   Updated: 2025/11/05 13:11:27 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

char	*resolve_command_path(char *cmd, t_shell *shell)
{
	char	*path;

	if (has_slash(cmd))
		path = ft_strdup(cmd);
	else
		path = find_executable(cmd, shell->env);
	return (path);
}

int	handle_path_errors(char *cmd, char *path, t_shell *shell)
{
	if (!path)
	{
		if (access(cmd, F_OK) == 0 && access(cmd, X_OK) != 0)
		{
			ft_putstr_fd("minishell: ", 2);
			ft_putstr_fd(cmd, 2);
			ft_putendl_fd(": Permission denied", 2);
			shell->exit_status = 126;
			return (0);
		}
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(cmd, 2);
		ft_putendl_fd(": command not found", 2);
		shell->exit_status = 127;
		return (0);
	}
	return (1);
}

int	check_execution_permission(char *cmd, char *path, t_shell *shell)
{
	if (is_directory(path))
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(cmd, 2);
		ft_putendl_fd(": is a directory", 2);
		shell->exit_status = 126;
		return (0);
	}
	if (access(path, F_OK) == 0 && access(path, X_OK) != 0)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(cmd, 2);
		ft_putendl_fd(": Permission denied", 2);
		shell->exit_status = 126;
		return (0);
	}
	return (1);
}
