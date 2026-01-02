/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_path.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 00:00:00 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/12 15:39:20 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"
/*
** find_executable - Locate executable file in PATH or validate path
** This function searches for an executable command in the system PATH or
** validates an absolute/relative path. It implements the following logic:
** - If command contains '/', treat as direct path and validate
** - Otherwise, search through PATH environment variable
** - Returns allocated string with full path or NULL if not found
*/
char	*find_executable(char *cmd, t_env *env)
{
	char	*path;

	if (!cmd || !*cmd)
		return (NULL);
	if (ft_strchr(cmd, '/'))
	{
		if (is_exec_file(cmd))
			return (ft_strdup(cmd));
		return (NULL);
	}
	path = get_env_value(env, "PATH");
	return (search_in_path(path, cmd));
}
