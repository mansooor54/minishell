/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_path.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/05 00:00:00 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/05 15:48:07 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** find_executable - Locate executable file in PATH or validate path
**
** This function searches for an executable command in the system PATH or
** validates an absolute/relative path. It implements the following logic:
**
** 1. If the command contains a '/' character, it's treated as an absolute
**    or relative path and returned as-is (after duplication)
** 2. Otherwise, it searches through all directories in the PATH environment
**    variable to find an executable file with the given name
** 3. Uses access() with X_OK to verify execute permissions
**
** The function allocates memory for the full path which must be freed by
** the caller.
**
** @param cmd: The command name to search for (e.g., "ls" or "/bin/ls")
** @param env: Linked list of environment variables containing PATH
**
** Return: Allocated string with full path to executable, or NULL if not found
**         - NULL if cmd is NULL or empty
**         - Duplicated cmd if it contains '/'
**         - Full path from PATH if found and executable
**         - NULL if PATH doesn't exist or command not found
*/

/* join two parts like "dir" + "/" + cmd */
char	*join_cmd_path(char *dir, char *cmd)
{
	char	*tmp;
	char	*full;

	tmp = ft_strjoin(dir, "/");
	if (!tmp)
		return (NULL);
	full = ft_strjoin_free(tmp, cmd);
	return (full);
}

/* ≤25 lines total */
char	*find_executable(char *cmd, t_env *env)
{
	char	*path_env;
	char	**paths;
	char	*full;
	int		i;

	if (!cmd || !*cmd)
		return (NULL);
	if (ft_strchr(cmd, '/'))
		return (ft_strdup(cmd));
	path_env = get_env_value(env, "PATH");
	if (!path_env)
		return (NULL);
	paths = ft_split(path_env, ':');
	if (!paths)
		return (NULL);
	i = 0;
	while (paths[i])
	{
		full = join_cmd_path(paths[i++], cmd);
		if (full && access(full, X_OK) == 0)
			return (free_array(paths), full);
		free(full);
	}
	free_array(paths);
	return (NULL);
}
