/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_path_search.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 00:00:00 by your_login        #+#    #+#             */
/*   Updated: 2025/11/05 13:11:27 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

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
char	*search_in_path(const char *path, const char *cmd)
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
		if (hit)
			return (hit);
		if (!path[j])
			break ;
		i = j + 1;
	}
	return (NULL);
}
