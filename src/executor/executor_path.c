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
**
*/

/* regular file + X_OK */
static int	is_exec_file(const char *path)
{
	struct stat	st;

	if (!path)
		return (0);
	if (stat(path, &st) != 0)
		return (0);
	if (!S_ISREG(st.st_mode))
		return (0);
	if (access(path, X_OK) != 0)
		return (0);
	return (1);
}

/* dir + "/" + cmd */
static char	*join_cmd_path(const char *dir, const char *cmd)
{
	size_t	a;
	size_t	b;
	char	*s;

	a = ft_strlen(dir);
	b = ft_strlen(cmd);
	s = malloc(a + b + 2);
	if (!s)
		return (NULL);
	ft_memcpy(s, dir, a);
	s[a] = '/';
	ft_memcpy(s + a + 1, cmd, b);
	s[a + b + 1] = '\0';
	return (s);
}

/* end index of PATH segment */
static size_t	seg_end(const char *path, size_t start)
{
	size_t	i;

	i = start;
	while (path[i] && path[i] != ':')
		i++;
	return (i);
}

/* "." if empty, else substring */
static char	*dup_segment_or_dot(const char *path, size_t start, size_t end)
{
	if (end == start)
		return (ft_strdup("."));
	return (ft_substr(path, start, end - start));
}

/* try dir/cmd and return full if executable */
static char	*probe_dir_for_cmd(const char *dir, const char *cmd)
{
	char	*full;

	full = join_cmd_path(dir, cmd);
	if (!full)
		return (NULL);
	if (is_exec_file(full))
		return (full);
	free(full);
	return (NULL);
}

/* iterate PATH entries */
static char	*search_in_path(const char *path, const char *cmd)
{
	size_t	i;
	size_t	j;
	char	*dir;
	char	*hit;

	if (!path)
		return (NULL);
	i = 0;
	while (1)
	{
		j = seg_end(path, i);
		dir = dup_segment_or_dot(path, i, j);
		if (!dir)
			return (NULL);
		hit = probe_dir_for_cmd(dir, cmd);
		free(dir);
		if (hit) return (hit);
		if (!path[j])
			break ;
		i = j + 1;
	}
	return (NULL);
}

/* top-level resolver */
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
