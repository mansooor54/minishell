/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   reader.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/13 12:41:02 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* Count consecutive backslashes immediately before index i (exclusive).
static size_t count_trailing_backslashes(const char *s, size_t i)
{
    size_t k = 0;
    while (i > 0 && s[i - 1] == '\\')
    {
        k++;
        i--;
    }
    return k;
}
*/

/* outside quotes: continue ONLY if an odd number of trailing '\' */
static int trailing_backslash_needs_more(const char *s)
{
	size_t i;
	int    backslashes;
	char   in_quote = 0;
	int    escaped = 0;

	if (!s || !*s)
		return 0;

	/* single pass to see if we end inside a quote */
	for (i = 0; s[i]; )
	{
		if (!in_quote && (s[i] == '\'' || s[i] == '"'))
		{
			in_quote = s[i++];
			escaped = 0;
			continue;
		}
		if (in_quote)
		{
			if (s[i] == in_quote && !escaped)
			{
				in_quote = 0;
				i++;
				continue;
			}
			escaped = (in_quote == '"' && s[i] == '\\' && !escaped);
			i++;
			continue;
		}
		/* outside quotes: skip escaped char pairs */
		if (s[i] == '\\')
		{
			if (s[i + 1] != '\0') i += 2;
			else i++;
			continue;
		}
		i++;
	}
	if (in_quote)
		return 1;

	/* count consecutive trailing backslashes ignoring spaces/tabs */
	i = ft_strlen(s);
	while (i > 0 && (s[i - 1] == ' ' || s[i - 1] == '\t'))
		i--;
	backslashes = 0;
	while (i > 0 && s[i - 1] == '\\')
	{
		backslashes++;
		i--;
	}
	return (backslashes % 2) == 1; /* continue only if odd */
}

static int needs_continuation(const char *s)
{
	if (has_unclosed_quotes((char *)s))
		return 1;
	return trailing_backslash_needs_more(s);
}

/* remove exactly ONE trailing '\' when continuing, then concatenate */
static char *join_continuation(char *line, char *next)
{
	size_t len;
	char   *tmp;
	char   *nptr = next ? next : "";

	if (!line)
		return ft_strdup(nptr);
	len = ft_strlen(line);
	while (len && (line[len - 1] == ' ' || line[len - 1] == '\t'))
		len--;
	if (len && line[len - 1] == '\\')
		line[len - 1] = '\0';
	tmp = ft_strjoin(line, nptr);
	free(line);
	return tmp;
}

/*
** \001 and \002 are control characters (SOH and STX).
** Readline uses them to know “this part does not take cursor space”.
*/
char	*read_logical_line(void)
{
	char	*line;
	char	*more;

	line = readline("\001\033[1;33m\002minishell> \001\033[0m\002");
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
