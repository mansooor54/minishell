/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   reader.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/05 11:20:00 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/* outside quotes: ends with unescaped '\' after optional spaces */
static int	trailing_backslash_needs_more(const char *s)
{
	size_t	i;

	if (!s || !*s)
		return (0);
	i = ft_strlen(s);
	while (i > 0 && (s[i - 1] == ' ' || s[i - 1] == '\t'))
		i--;
	if (i == 0)
		return (0);
	return (s[i - 1] == '\\');
}

static int	needs_continuation(const char *s)
{
	if (has_unclosed_quotes((char *)s))
		return (1);
	return (trailing_backslash_needs_more(s));
}

static char	*join_continuation(char *line, char *next)
{
	size_t	len;
	char	*tmp;
	char	*nptr;

	if (next)
		nptr = next;
	else
		nptr = "";
	if (!line)
		return (ft_strdup(nptr));
	len = ft_strlen(line);
	while (len && (line[len - 1] == ' ' || line[len - 1] == '\t'))
		len--;
	if (len && line[len - 1] == '\\')
		line[len - 1] = '\0';
	tmp = ft_strjoin(line, nptr);
	free(line);
	return (tmp);
}

char	*read_logical_line(void)
{
	char	*line;
	char	*more;

	line = readline("\033[1;33mminishell> \033[0m");
	if (!line)
		return (NULL);
	while (needs_continuation(line))
	{
		more = readline("> ");
		if (!more)
			break ;
		line = join_continuation(line, more);
		free(more);
	}
	return (line);
}
