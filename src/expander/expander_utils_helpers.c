/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_utils_helpers.c                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/23 12:00:00 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/23 12:00:00 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

static int	count_non_empty_args(char **args)
{
	int	i;
	int	cnt;

	cnt = 0;
	i = 0;
	while (args[i])
	{
		if (args[i][0] != '\0')
			cnt++;
		i++;
	}
	return (cnt);
}

static char	**allocate_compact_args(char **args, int cnt)
{
	char	**new;
	int		i;
	int		j;

	new = malloc(sizeof(char *) * (cnt + 1));
	if (!new)
		return (args);
	i = 0;
	j = 0;
	while (args[i])
	{
		if (args[i][0] != '\0')
			new[j++] = args[i];
		else
			free(args[i]);
		i++;
	}
	new[j] = NULL;
	free(args);
	return (new);
}

/* Expose helpers for other files */
int	get_non_empty_arg_count(char **args)
{
	return (count_non_empty_args(args));
}

char	**get_allocated_compact_args(char **args, int cnt)
{
	return (allocate_compact_args(args, cnt));
}
