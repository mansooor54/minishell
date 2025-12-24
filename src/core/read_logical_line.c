/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   read_logical_line.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/20 21:00:00 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

static char	*read_one_line(const char *prompt)
{
	char	*line;

	g_signal = 0;
	rl_done = 0;
	line = readline(prompt);
	if (g_signal == SIGINT)
	{
		if (line)
			free(line);
		return (NULL);
	}
	return (line);
}

static char	*handle_continuation_error(char *line)
{
	free(line);
	ft_putendl_fd("minishell: syntax error: unexpected end of file", 2);
	return (ft_strdup(""));
}

static char	*process_continuation(char *line)
{
	char	*more;

	more = read_one_line("> ");
	if (g_signal == SIGINT)
	{
		free(line);
		if (more)
			free(more);
		return (NULL);
	}
	if (!more)
		return (handle_continuation_error(line));
	line = join_continuation(line, more);
	free(more);
	return (line);
}

char	*read_logical_line(void)
{
	char	*line;

	line = read_one_line("\001\033[1;33m\002minishell> \001\033[0m\002");
	if (!line)
		return (NULL);
	while (needs_continuation(line))
	{
		line = process_continuation(line);
		if (!line)
			return (NULL);
	}
	return (line);
}
