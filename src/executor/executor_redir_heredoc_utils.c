/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_redir_heredoc_utils.c                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/23 00:00:00 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/23 11:48:00 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

char	*clean_delimiter(char *delim, int *quoted)
{
	*quoted = (ft_strchr(delim, '\'') || ft_strchr(delim, '"'));
	if (*quoted)
		return (remove_quotes(delim));
	return (ft_strdup(delim));
}

int	check_heredoc_end(char *line, char *clean)
{
	if (!line || ft_strcmp(line, clean) == 0)
	{
		if (!line)
		{
			ft_putstr_fd("minishell: warning: here-document ", 2);
			ft_putstr_fd("delimited by end-of-file (wanted '", 2);
			ft_putstr_fd(clean, 2);
			ft_putendl_fd("')", 2);
		}
		return (1);
	}
	return (0);
}

char	*get_expanded_line(char *line, int quoted, t_env *env, int exit_status)
{
	if (quoted)
		return (ft_strdup(line));
	return (expand_variables(line, env, exit_status));
}
