/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_operator.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: malmarzo <malmarzo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/02 09:15:37 by malmarzo          #+#    #+#             */
/*   Updated: 2025/11/05 16:02:34 by malmarzo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

/* split to avoid >25 lines */
static t_token	*try_or_pipe(char **input)
{
	if (**input != '|')
		return (NULL);
	if (*(*input + 1) == '|')
	{
		*input += 2;
		return (create_token(TOKEN_OR, "||"));
	}
	(*input)++;
	return (create_token(TOKEN_PIPE, "|"));
}

static t_token	*try_and(char **input)
{
	if (**input == '&' && *(*input + 1) == '&')
	{
		*input += 2;
		return (create_token(TOKEN_AND, "&&"));
	}
	return (NULL);
}

static t_token	*try_inredir(char **input)
{
	if (**input == '<' && *(*input + 1) == '<')
	{
		*input += 2;
		return (create_token(TOKEN_REDIR_HEREDOC, "<<"));
	}
	if (**input == '<')
	{
		(*input)++;
		return (create_token(TOKEN_REDIR_IN, "<"));
	}
	return (NULL);
}

static t_token	*try_outredir(char **input)
{
	if (**input == '>' && *(*input + 1) == '>')
	{
		*input += 2;
		return (create_token(TOKEN_REDIR_APPEND, ">>"));
	}
	if (**input == '>')
	{
		(*input)++;
		return (create_token(TOKEN_REDIR_OUT, ">"));
	}
	return (NULL);
}

/*
** Identify and create operator token
** Handles |, ||, &&, <, <<, >, >>
*/
t_token	*get_operator_token(char **input)
{
	t_token	*tok;

	tok = try_or_pipe(input);
	if (tok)
		return (tok);
	tok = try_and(input);
	if (tok)
		return (tok);
	tok = try_inredir(input);
	if (tok)
		return (tok);
	return (try_outredir(input));
}
