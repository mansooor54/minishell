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

static int	has_unclosed_quotes_in_string(const char *s)
{
	size_t	i;
	char	in_quote;
	int		escaped;

	i = 0;
	in_quote = 0;
	escaped = 0;
	while (s[i] != '\0')
	{
		if (!in_quote && (s[i] == '\'' || s[i] == '"'))
			in_quote = s[i];
		else if (in_quote && s[i] == in_quote && !escaped)
			in_quote = 0;
		if (in_quote == '"' && s[i] == '\\' && !escaped)
			escaped = 1;
		else if (escaped)
			escaped = 0;
		i++;
	}
	return (in_quote != 0);
}

static int	find_trailing_backslash(const char *s)
{
	int	i;

	i = ft_strlen(s) - 1;
	while (i >= 0 && (s[i] == ' ' || s[i] == '\t'))
		i--;
	if (i >= 0 && s[i] == '\\')
		return (1);
	return (0);
}

static int	trailing_backslash_needs_more(const char *s)
{
	if (!s || !*s)
		return (0);
	if (has_unclosed_quotes_in_string(s))
		return (1);
	return (find_trailing_backslash(s));
}

int	needs_continuation(const char *s)
{
	if (has_unclosed_quotes((char *)s))
		return (1);
	return (trailing_backslash_needs_more(s));
}
