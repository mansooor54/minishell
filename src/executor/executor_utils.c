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

#include "minishell.h"

static inline void	put2(const char *s)
{
	if (s)
		write(2, s, ft_strlen(s));
}

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

int	check_execution_permission(char *cmd, char *path, t_shell *shell)
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
