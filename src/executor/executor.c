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
