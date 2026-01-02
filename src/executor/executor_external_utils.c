/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_external_utils.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 12:00:00 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/24 12:00:00 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

int	handle_directory_check(char *cmd, t_shell *shell)
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

int	handle_direct_path(char *cmd, t_shell *shell, char **path)
{
	if (handle_directory_check(cmd, shell))
		return (1);
	if (access(cmd, F_OK) != 0)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(cmd, 2);
		ft_putstr_fd(": ", 2);
		ft_putendl_fd(strerror(errno), 2);
		shell->exit_status = 127;
		return (1);
	}
	if (access(cmd, X_OK) != 0)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(cmd, 2);
		ft_putstr_fd(": ", 2);
		ft_putendl_fd(strerror(errno), 2);
		shell->exit_status = 126;
		return (1);
	}
	*path = ft_strdup(cmd);
	return (0);
}

int	handle_path_resolution(t_cmd *cmd, t_shell *shell, char **path)
{
	if (ft_strchr(cmd->args[0], '/'))
	{
		if (handle_direct_path(cmd->args[0], shell, path))
			return (1);
	}
	else
		*path = find_executable(cmd->args[0], shell->env);
	if (!*path)
	{
		cmd_not_found(cmd->args[0]);
		shell->exit_status = 127;
		return (1);
	}
	return (0);
}
