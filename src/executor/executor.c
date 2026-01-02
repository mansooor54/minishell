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

#include "../../include/minishell.h"

static void	handle_execve_error(char **argv, t_shell *shell)
{
	if (errno == EACCES || errno == ENOEXEC)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(argv[0], 2);
		ft_putendl_fd(": Permission denied", 2);
		shell->exit_status = 126;
	}
	else
	{
		ft_putstr_fd("minishell: ", 2);
		perror(argv[0]);
		shell->exit_status = 127;
	}
}

void	execute_command(char **argv, t_shell *shell, char **envp)
{
	char	*path;

	path = resolve_command_path(argv[0], shell);
	if (!handle_path_errors(argv[0], path, shell))
	{
		if (path)
			free(path);
		return ;
	}
	if (!check_execution_permission(argv[0], path, shell))
	{
		free(path);
		return ;
	}
	execve(path, argv, envp);
	handle_execve_error(argv, shell);
	free(path);
}
