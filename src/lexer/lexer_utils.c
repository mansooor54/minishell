/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: student <student@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 00:00:00 by student           #+#    #+#             */
/*   Updated: 2025/11/02 00:00:00 by student          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/*
** Check if character is a whitespace (space or tab)
*/
static int	is_whitespace(char c)
{
	return (c == ' ' || c == '\t');
}

/*
** Check if character is a special operator character
** Returns 1 for |, <, >, &
*/
static int	is_operator(char c)
{
	return (c == '|' || c == '<' || c == '>' || c == '&');
}

/*
** Extract a word token from input, handling quotes
** Returns the length of the word extracted
*/
static int	extract_word(char *input, char **word)
{
	int		i;
	int		in_quote;
	char	quote_char;

	i = 0;
	in_quote = 0;
	while (input[i] && (in_quote || (!is_whitespace(input[i])
				&& !is_operator(input[i]))))
	{
		if (!in_quote && (input[i] == '\'' || input[i] == '"'))
		{
			quote_char = input[i];
			in_quote = 1;
		}
		else if (in_quote && input[i] == quote_char)
			in_quote = 0;
		i++;
	}
	*word = malloc(i + 1);
	if (!*word)
		return (0);
	ft_strncpy(*word, input, i);
	(*word)[i] = '\0';
	return (i);
}

/*
** Identify and create operator token
** Handles |, ||, &&, <, <<, >, >>
*/
static t_token	*get_operator_token(char **input)
{
	if (**input == '|' && *(*input + 1) == '|')
	{
		(*input) += 2;
		return (create_token(TOKEN_OR, "||"));
	}
	if (**input == '|')
		return ((*input)++, create_token(TOKEN_PIPE, "|"));
	if (**input == '&' && *(*input + 1) == '&')
	{
		(*input) += 2;
		return (create_token(TOKEN_AND, "&&"));
	}
	if (**input == '<' && *(*input + 1) == '<')
	{
		(*input) += 2;
		return (create_token(TOKEN_REDIR_HEREDOC, "<<"));
	}
	if (**input == '<')
		return ((*input)++, create_token(TOKEN_REDIR_IN, "<"));
	if (**input == '>' && *(*input + 1) == '>')
	{
		(*input) += 2;
		return (create_token(TOKEN_REDIR_APPEND, ">>"));
	}
	if (**input == '>')
		return ((*input)++, create_token(TOKEN_REDIR_OUT, ">"));
	return (NULL);
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
