/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/05 16:04:30 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

static int	is_whitespace(char c)
{
	return (c == ' ' || c == '\t');
}

static int	is_operator(char c)
{
	return (c == '|' || c == '<' || c == '>' || c == '&');
}

/*
** Extract a word token from input, handling quotes
** Returns the length of the word extracted
*/
/* returns length consumed, writes raw slice (quotes/backslashes kept) */
static int	extract_word(char *input, char **word)
{
	int		i;
	int		in_quote;     /* 0 or '\'' or '"' */

	i = 0;
	in_quote = 0;
	while (input[i] && (in_quote || (!is_whitespace(input[i])
				&& !is_operator(input[i]))))
	{
		/* enter quote if not already in quotes and we see a quote */
		if (!in_quote && (input[i] == '\'' || input[i] == '"'))
		{
			in_quote = input[i++];      /* track we’re inside quotes */
			continue ;
		}

		/* leave quotes:
		   - single quotes: never escaped → close on matching '
		   - double quotes: close on " only if previous char is NOT a backslash
		*/
		if (in_quote)
		{
			if (input[i] == in_quote)
			{
				if (in_quote == '"' && i > 0 && input[i - 1] == '\\')
				{
					/* \" does NOT close; keep both chars for remove_quotes */
					i++;
					continue ;
				}
				in_quote = 0;
				i++;
				continue ;
			}
			i++;
			continue ;
		}
		/* outside quotes: backslash stays; handled later */
		i++;
	}
	*word = malloc((size_t)i + 1);
	if (!*word)
		return (0);
	ft_strncpy(*word, input, i);
	(*word)[i] = '\0';
	return (i);
}

/*
** Main lexer function
** Tokenizes input string into a linked list of tokens
*/
t_token	*lexer(char *input)
{
	t_token	*tokens;
	t_token	*new_token;
	char	*word;
	int		len;

	tokens = NULL;
	while (*input)
	{
		while (is_whitespace(*input))
			input++;
		if (!*input)
			break ;
		if (is_operator(*input))
			new_token = get_operator_token(&input);
		else
		{
			len = extract_word(input, &word);
			new_token = create_token(TOKEN_WORD, word);
			free(word);
			input += len;
		}
		if (new_token)
			add_token(&tokens, new_token);
	}
	return (tokens);
}
