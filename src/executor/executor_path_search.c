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

#include "../../include/minishell.h"

static char	*check_and_store_path(char *full, char **found_non_exec)
{
	if (full && is_exec_file(full))
	{
		if (*found_non_exec)
			free(*found_non_exec);
		return (full);
	}
	if (full && !*found_non_exec && access(full, F_OK) == 0)
	{
		if (!is_directory(full))
			*found_non_exec = full;
		else
			free(full);
	}
	else
		free(full);
	return (NULL);
}

char	*search_in_path(const char *path, const char *cmd)
{
	size_t	i;
	size_t	j;
	char	*dir;
	char	*full;
	char	*found_non_exec;

	if (!path)
		return (NULL);
	i = 0;
	found_non_exec = NULL;
	while (1)
	{
		j = seg_end(path, i);
		dir = dup_segment_or_dot(path, i, j);
		if (!dir)
			return (found_non_exec);
		full = join_cmd_path(dir, cmd);
		free(dir);
		if (check_and_store_path(full, &found_non_exec))
			return (full);
		if (!path[j])
			break ;
		i = j + 1;
	}
	return (found_non_exec);
}
