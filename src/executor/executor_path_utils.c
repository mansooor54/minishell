/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_child_fds.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 00:00:00 by your_login        #+#    #+#             */
/*   Updated: 2025/11/05 13:11:27 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/* regular file + X_OK */
int	is_exec_file(const char *path)
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
char	*join_cmd_path(const char *dir, const char *cmd)
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
size_t	seg_end(const char *path, size_t start)
{
	size_t	i;

	i = start;
	while (path[i] && path[i] != ':')
		i++;
	return (i);
}

/* "." if empty, else substring */
char	*dup_segment_or_dot(const char *path, size_t start, size_t end)
{
	if (end == start)
		return (ft_strdup("."));
	return (ft_substr(path, start, end - start));
}
