/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   reader.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/13 15:08:35 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** \001 and \002 are control characters (SOH and STX).
** Readline uses them to know “this part does not take cursor space”.
** \001 and \002: tell readline that the color codes don't take cursor space
*/
static char	*read_initial_line(void)
{
	char	*line;

	g_shell.sigint_during_read = 0;
	line = readline("\001\033[1;33m\002minishell> \001\033[0m\002");
	if (!line)
		return (NULL);
	if (g_shell.sigint_during_read)
	{
		free(line);
		g_shell.exit_status = 130;
		return (NULL);
	}
	return (line);
}

static char	*handle_continuation_line(char *line)
{
	char	*more;

	g_shell.sigint_during_read = 0;
	more = readline("> ");
	if (g_shell.sigint_during_read)
	{
		if (more)
			free(more);
		free(line);
		return (NULL);
	}
	if (!more)
	{
		free(line);
		return (NULL);
	}
	return (more);
}

char	*join_continuation(char *line, char *next)
{
	size_t	len;
	char	*tmp;
	char	*nptr;

	nptr = next;
	if (!nptr)
		nptr = "";
	if (!line)
		return (ft_strdup(nptr));
	len = ft_strlen(line);
	while (len > 0 && (line[len - 1] == ' ' || line[len - 1] == '\t'))
		len--;
	if (len > 0 && line[len - 1] == '\\')
		line[len - 1] = '\0';
	tmp = ft_strjoin(line, nptr);
	free(line);
	return (tmp);
}

static char	*process_continuation(char *line)
{
	char	*more;
	char	*new_line;

	while (needs_continuation(line))
	{
		more = handle_continuation_line(line);
		if (!more)
			return (NULL);
		new_line = join_continuation(line, more);
		free(more);
		if (!new_line)
			return (NULL);
		line = new_line;
	}
	return (line);
}

char	*read_logical_line(void)
{
	char	*line;

	line = read_initial_line();
	if (!line)
		return (NULL);
	line = process_continuation(line);
	return (line);
}
