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
	return (c == '|' || c == '<' || c == '>' || c == '&');
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

// /* Collapse escapes outside quotes:  \\x -> \x,  \\ -> \  */
// static char *unescape_outside_quotes(const char *s)
// {
// 	int   i = 0, j = 0;
// 	char  inq = 0;
// 	char *out = malloc(ft_strlen(s) + 1);

// 	if (!out) return NULL;
// 	while (s[i])
// 	{
// 		if (!inq && (s[i] == '\'' || s[i] == '"'))
// 		{
// 			inq = s[i++];
// 			continue;                 /* remove quote chars */
// 		}
// 		if (inq && s[i] == inq)      /* closing quote */
// 		{
// 			inq = 0;
// 			i++;
// 			continue;                 /* remove quote chars */
// 		}
// 		if (!inq && s[i] == '\\' && s[i + 1] != '\0')
// 		{
// 			out[j++] = s[i + 1];     /* eat the backslash, keep next char */
// 			i += 2;
// 			continue;
// 		}
// 		out[j++] = s[i++];
// 	}
// 	out[j] = '\0';
// 	return out;
// }

/* alloc + copy */
// int	extract_word(char *input, char **word)
// {
// 	int   len;
// 	char *raw;
// 	char *un;

// 	len = measure_word(input);                                /* scans span */
// 	raw = malloc((size_t)len + 1);
// 	if (!raw)
// 		return 0;
// 	ft_strncpy(raw, input, len);
// 	raw[len] = '\0';

// 	un = unescape_outside_quotes(raw);                        /* NEW */
// 	free(raw);
// 	if (!un)
// 		return 0;

// 	*word = un;
// 	return len;                                              /* advance input */
// }

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