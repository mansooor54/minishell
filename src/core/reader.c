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
/* Return 1 if line ends with a backslash outside quotes → needs continuation */
static int	trailing_backslash_needs_more(const char *s)
{
	size_t	i;
	char	in_quote;
	int		escaped;

	if (!s || !*s)
		return (0);
	i = 0;
	in_quote = 0;
	escaped = 0;
	while (s[i] != '\0')
	{
		if (!in_quote && (s[i] == '\'' || s[i] == '"'))
		{
			in_quote = s[i];
			escaped = 0;
		}
		else if (in_quote)
		{
			if (s[i] == in_quote && !escaped)
				in_quote = 0;
			if (in_quote == '"' && s[i] == '\\' && !escaped)
				escaped = 1;
			else
				escaped = 0;
		}
		i++;
	}
	if (in_quote)
		return (1);
	i = ft_strlen(s);
	while (i > 0 && (s[i - 1] == ' ' || s[i - 1] == '\t'))
		i--;
	if (i == 0)
		return (0);
	if (s[i - 1] == '\\')
		return (1);
	return (0);
}

static int	needs_continuation(const char *s)
{
	if (has_unclosed_quotes((char *)s))
		return (1);
	return (trailing_backslash_needs_more(s));
}

/* Remove one trailing '\' (if any), then append the next line */
static char	*join_continuation(char *line, char *next)
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
		{
			/* Ctrl-D or EOF during continuation → cancel whole command */
			free(line);
			return (NULL);
		}
		/* Ctrl-C: handler sets exit_status = 130; readline returns "" */
		if (g_shell.exit_status == 130 && more[0] == '\0')
		{
			free(more);
			free(line);
			return (NULL);
		}
		line = join_continuation(line, more);
		free(more);
	}
	return (line);
}