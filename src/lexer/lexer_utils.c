/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/13 12:21:28 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	is_whitespace(char c)
{
	return (c == ' ' || c == '\t');
}

int	is_operator(char c)
{
	if (!c)
		return (0);
	if (c == '|')
		return (1);
	if (c == '>')
		return (1);
	if (c == '<')
		return (1);
	if (c == '&')
		return (1);
	if (c == ';')
		return (1);
	return (0);
}

static int	is_word_cont(char *s, int i, int in_quote)
{
	if (!s[i])
		return (0);
	if (in_quote)
		return (1);
	if (is_whitespace(s[i]) || is_operator(s[i]))
		return (0);
	return (1);
}

/* scans only, finds length */
static int	measure_word(char *s)
{
	int	i;
	int	in_quote;

	i = 0;
	in_quote = 0;
	while (is_word_cont(s, i, in_quote))
	{
		if (!in_quote && (s[i] == '\'' || s[i] == '"'))
		{
			in_quote = s[i];
			i++;
			continue ;
		}
		if (in_quote && s[i] == in_quote)
		{
			if (!(in_quote == '"' && i > 0 && s[i - 1] == '\\'))
				in_quote = 0;
			i++;
			continue ;
		}
		i++;
	}
	return (i);
}

int	extract_word(char *input, char **word)
{
	int	len;

	len = measure_word(input);
	*word = malloc((size_t)len + 1);
	if (!*word)
		return (0);
	ft_strncpy(*word, input, len);
	(*word)[len] = '\0';
	return (len);
}
